#include "config.hpp"
#include "shader/shader.hpp"
#include "drawer/cube.hpp"
#include "time/time.hpp"
#include "components/transform.hpp"
#include "input/input.hpp"
#include "drawer/line.hpp"
#include "drawer/extras/axis.hpp"
#include "drawer/plane.hpp"
#include <GLFW/glfw3native.h>
#include "../assets/icon.h" 

static const std::string vert_name = "test";
static const std::string frag_name = "test";

static const std::string lines_vert_name = "vs_flat";
static const std::string lines_frag_name = "fs_flat";

static bgfx::UniformHandle u_lightPosIntensity     ;
static bgfx::UniformHandle u_cameraPosShininess;
static bgfx::UniformHandle u_albedo            ;
static bgfx::UniformHandle u_TimeDeltaMouseXY            ;

struct WindowSettings{
    uint16_t window_width = 1280;
    uint16_t window_height = 720;
    std::string window_title = "My Render Engine";
};


void init_bgfx(GLFWwindow * win, bgfx::Init* init){
    
        // Fill platform data for bgfx
        bgfx::PlatformData pd{};
#ifdef _WIN32
    HWND nwh = glfwGetWin32Window(win);
    void* ndt = nullptr;
#else
    #ifdef GLFW_EXPOSE_NATIVE_X11
        Display* ndt = glfwGetX11Display();
        ::Window nwhX11 = glfwGetX11Window(win);
        void* nwh = (void*)(uintptr_t)nwhX11;
    #elif defined(GLFW_EXPOSE_NATIVE_WAYLAND)
        struct wl_display* ndt = glfwGetWaylandDisplay();
        struct wl_surface* nwh = glfwGetWaylandWindow(win);
    #endif
#endif
    pd.nwh = nwh;
    
    int fbw, fbh; glfwGetFramebufferSize(win, &fbw, &fbh);
    
    init->type = bgfx::RendererType::Vulkan; // pick Vulkan; use Count for auto
    init->platformData = pd;
    init->resolution.width  = (uint32_t)fbw;
    init->resolution.height = (uint32_t)fbh;
    //init->resolution.reset  = BGFX_RESET_VSYNC;
    
    if (!bgfx::init(*init)) {
        std::fprintf(stderr, "bgfx::init failed\n");
        exit(EXIT_FAILURE);
    }
    
}

void init_engine(GLFWwindow*& win, WindowSettings& ws, bgfx::Init& init){
    // < --- window init settings --- > // 
    
    if (!glfwInit()) { throw std::runtime_error("GLFW init failed\n"); }
    // We don't want GLFW to create an OpenGL context. bgfx will own the GPU.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // make the window invisible until first frame executed.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    win = glfwCreateWindow(ws.window_width, ws.window_height, ws.window_title.c_str(), nullptr, nullptr);
    if (!win) { throw std::runtime_error("Window creation failed\n"); }
    // < --- end window init settings --- > //
    
    

    GLFWimage images[1];
    images[0].width  =  ICON_WIDTH;
    images[0].height =  ICON_HEIGHT;
    images[0].pixels = icon_pixels;
    glfwSetWindowIcon(win, 1, images);



    // init bgfx
    init_bgfx(win, &init);
}

void shoutdown_and_cleanup(GLFWwindow * win){
    Shader::shader::Instance().cleanup();
    bgfx::shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();
}

void handle_resize(GLFWwindow * win, bgfx::Init& init, int& w, int& h){
    glfwGetFramebufferSize(win, &w, &h);
    if (w != (int)init.resolution.width || h != (int)init.resolution.height) {
        init.resolution.width  = (uint32_t)w;
        init.resolution.height = (uint32_t)h;
        bgfx::reset(init.resolution.width, init.resolution.height, init.resolution.reset);
    }
}

 
void draw_axis(){

    // -------- axis -----------
    static axis ax(XYZ_AXIS);
    ax.set_transform(Components::transform({0,0,0}, {0,0,0}, {1,1,1}));
    ax.set_length(20.0f);                         
    ax.set_axes_colors(0xff0000ff, 0xff00ff00, 0xffff0000); // X=red, Y=green, Z=blue

    ax.enable_floor(true);
    ax.set_floor_plane(XZ_AXIS);
    ax.set_floor_lines_count(50);                 
    ax.set_floor_color(0xff5a5a5a);
    ax.create_renderer_lines(lines_vert_name, lines_frag_name); 
     
}

static bx::Vec3 update_orbit_camera_blender(Input::input& in, int winW, int winH,
                                        float* outView /*16*/, float* outProj /*16*/)
{
    // persistent camera state (orbit around target)
    static float yaw   = 0.0f;            // radians, around + Y
    static float pitch = 0.0f;            // radians, around + X
    static float dist  = 10.0f;           // distance from target
    static math::Vec3 target{0.0f, 0.0f, 0.0f};

    // tunables
    const float rotateSpeed = 0.005f;     // radians per pixel
    const float minDist     = 0.2f;
    const float maxDist     = 1000.0f;
    const float zoomStep    = 1.10f;      // exponential zoom per wheel tick

    // orbit when holding middle mouse
    if (in.is_mouse_down(GLFW_MOUSE_BUTTON_MIDDLE)) {
        math::Vec2 d = in.get_mouse_delta();         // per-frame mouse delta (px) 
        yaw   += d.x * rotateSpeed;                  // drag left → yaw left
        pitch -= d.y * rotateSpeed;                  // drag up → pitch up
        // clamp pitch to avoid flipping
        const float limit = bx::kPi * 0.5f - 0.001f;
        if (pitch >  limit) pitch =  limit;
        if (pitch < -limit) pitch = -limit;
    }

    // zoom with scroll wheel (positive = zoom in)
    math::Vec2 scroll = in.get_scroll_delta();       // per-frame scroll  
    if (scroll.y != 0.0f) {
        dist *= std::pow(zoomStep, -scroll.y);       // exponential dolly
        if (dist < minDist) dist = minDist;
        if (dist > maxDist) dist = maxDist;
    }

    // build eye from yaw/pitch/distance around target (forward +Z at yaw=0,pitch=0)
    const float cp = std::cos(pitch), sp = std::sin(pitch);
    const float cy = std::cos(yaw),   sy = std::sin(yaw);
    bx::Vec3 at  { target.x           , target.y           , target.z            };
    bx::Vec3 dir { cp * sy            , sp                 , cp * cy             };  // +Z when yaw=0,pitch=0
    bx::Vec3 eye { at.x - dir.x * dist, at.y - dir.y * dist, at.z - dir.z * dist };

    // view / proj
    bx::mtxLookAt(outView, eye, at, bx::Vec3{0.0f, 1.0f, 0.0f});  // up = +Y  
    bx::mtxProj(outProj, 60.0f, float(winW) / float(winH), 0.1f, 1000.0f,
                bgfx::getCaps()->homogeneousDepth); 
                
    u_lightPosIntensity  = bgfx::createUniform("u_lightPosIntensity" ,bgfx::UniformType::Vec4);
    u_cameraPosShininess = bgfx::createUniform("u_cameraPosShininess",bgfx::UniformType::Vec4);
    u_albedo             = bgfx::createUniform("u_albedo"            ,bgfx::UniformType::Vec4);
    u_TimeDeltaMouseXY   = bgfx::createUniform("u_TimeDeltaMouseXY"  ,bgfx::UniformType::Vec4);

    return eye;
}

void print_fps(){
     bgfx::setDebug(BGFX_DEBUG_TEXT);

        // Clear screen and debug text
        bgfx::dbgTextClear();
        // compute instantaneous values
        float dt  = TimeSystem::time::delta_time;       // seconds per frame
        float fps = dt > 0.0f ? 1.0f / dt : 0.0f;
        float ms  = dt * 1000.0f;

        // optional: smooth FPS with EMA
        static float fpsSmooth = 0.0f;
        const float alpha = 0.1f;                        // 0..1 (higher = less smoothing)
        fpsSmooth = (fpsSmooth == 0.0f) ? fps : (alpha*fps + (1.0f - alpha)*fpsSmooth);

        // print
        char buf[128];
        std::snprintf(buf, sizeof(buf), "FPS: %.1f   (%.2f ms)", fpsSmooth, ms);
        bgfx::dbgTextPrintf(1, 1, 0x9f, buf);
}

int main()
{
    WindowSettings ws{};
    GLFWwindow * win = nullptr;
    bgfx::Init init{};
    try{
        init_engine(win, ws, init);
    }catch(const std::runtime_error re){
        std::cout << re.what() << std:: endl;
        return EXIT_FAILURE;
    };
    Input::input input_manager(win);
    auto& inst = Shader::shader::Instance();
    TimeSystem::time::init();

    // ----------- simple rendering -------
    cube *light_source = new cube(0xffff00ff);
    light_source->create_renderer("light", "light");
    light_source->set_transform(Components::transform{
        math::Vec3{3,3,3}, 
        math::Vec3{0, 0, 0}, 
        math::Vec3{0.5,0.5,0.5}
    });
    plane pl{0xff0000ff};
    pl.create_renderer("plane", frag_name);
    Components::transform trans = pl.get_transform();
    pl.set_transform(Components::transform{math::Vec3{7.5, 0.1, -7.5}, trans.get_rotation(), math::Vec3{10}});


    int w = ws.window_width, h = ws.window_height;
    // show the window
    glfwShowWindow(win);
    float angle = 0;
    
    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        TimeSystem::time::update();
        input_manager.update();
        angle += TimeSystem::time::delta_time;

        // Resize handling
        handle_resize(win, init, w, h);

        // View 0 = full screen clear
        bgfx::setViewRect(0, 0, 0, (uint16_t)init.resolution.width, (uint16_t)init.resolution.height);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x141414d9, 1.0f, 0);

        bgfx::touch(0); // ensure view is valid even if we submit nothing

        float view[16], proj[16];
        bx::Vec3 eye = update_orbit_camera_blender(input_manager, w, h, view, proj);
        bgfx::setViewTransform(0, view, proj);  
        math::Vec3 pos = light_source->get_transform().get_position();
       
        // light & material
        const bx::Vec3 lightDir = bx::normalize(bx::Vec3(-1.0f, 0.0f, -1.0f)); // world-space
        float lightDirColor[4]      = { pos.x, pos.y, pos.z,1.0f}; // w = intensity
        float cameraPosShininess[4] = { eye.x, eye.y , eye.z, 32.0f };                // .w = shininess
        float albedo[4]             = { 1.0f, 0.0f,1.0f, 1.0f };

        // rotate light sourcein circle with radius dis
        Components::transform tr = light_source->get_transform();
        float dis = 5;
        tr.set_position( math::Vec3{0,7,0} + math::Vec3{dis*bx::cos(angle), 0, dis*bx::sin(angle)});
        light_source->set_transform(tr);


        math::Vec2 p = input_manager.get_mouse_pos();
        float timeDeltaMouseXY[4] = {angle ,TimeSystem::time::delta_time, p.x, p.y};
        
        bgfx::setUniform(u_lightPosIntensity ,     lightDirColor);
        bgfx::setUniform(u_cameraPosShininess,cameraPosShininess);
        bgfx::setUniform(u_albedo            ,            albedo);
        bgfx::setUniform(u_TimeDeltaMouseXY  ,  timeDeltaMouseXY);
        
        print_fps();

        inst.update();

        bgfx::frame();
    }

    shoutdown_and_cleanup(win);
    return 0;
}
