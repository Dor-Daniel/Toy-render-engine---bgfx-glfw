// Includes
// ---------------
#include "config.hpp"
#include "shader/shader.hpp"
#include "drawer/cube.hpp"
#include "time/time.hpp"
#include "components/transform.hpp"
#include "input/input.hpp"
#include "drawer/line.hpp"
#include "drawer/extras/axis.hpp"
#include "drawer/plane.hpp"
#include "drawer/extras/sphere.cpp"
#include "drawer/extras/function_drawer.cpp"
#include <GLFW/glfw3native.h>
#include "../assets/icon.h" 
#include <thread>
#include <chrono>
//-------------------------------------------

// Globals
// ---------------
static GLFWwindow* win = nullptr;
static bgfx::Init init;
static bool running = true;
// ------------------------------

// Definitions
// ---------------
struct WindowSettings{
    uint16_t window_width = 1280;
    uint16_t window_height = 720;
    std::string window_title = "My Render Engine";
};


// Helpers
// ---------------
void init_bgfx(void){
    
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
    
    init.type = bgfx::RendererType::Vulkan; // pick Vulkan; use Count for auto
    init.platformData = pd;
    init.resolution.width  = (uint32_t)fbw;
    init.resolution.height = (uint32_t)fbh;
    //init.resolution.reset  = BGFX_RESET_VSYNC;
    
    if (!bgfx::init(init)) {
        std::fprintf(stderr, "bgfx::init failed\n");
        exit(EXIT_FAILURE);
    }
    
}

void draw_axis(void){
    // -------- axis -----------
    static axis ax(XYZ_AXIS);
    ax.set_transform(Components::transform({0,0,0}, {0,0,0}, {1,1,1}));
    ax.set_length(20.0f);                         
    ax.set_axes_colors(0xff0000ff, 0xff00ff00, 0xffff0000); // X=red, Y=green, Z=blue

    ax.enable_floor(true);
    ax.set_floor_plane(XZ_AXIS);
    ax.set_floor_lines_count(50);                 
    ax.set_floor_color(0xff5a5a5a);
    ax.create_renderer_lines("vs_flat", "fs_flat"); 
}

static bx::Vec3 update_orbit_camera_blender(const int winW, const int winH,
                                            float* outView /*16*/, float* outProj /*16*/)
{
    const Input::input& in = Input::input::get(win);

    // persistent camera state (orbit around target)
    static float yaw   = 0.0f;            // radians, around +Y
    static float pitch = 0.0f;            // radians, around +X
    static float dist  = 10.0f;           // distance from target
    static math::Vec3 target{0.0f, 0.0f, 0.0f};

    // track mouse button state to detect release
    static bool prevMMBDown = false;

    // tunables
    const float rotateSpeed = 0.005f;     // radians per pixel
    const float minDist     = 0.2f;
    const float maxDist     = 1000.0f;
    const float zoomStep    = 1.10f;      // exponential zoom per wheel tick
    const float panBase     = 0.0015f;    // world units per pixel at dist=1 (scaled by dist)

    const bool mmbDown   = in.is_mouse_down(GLFW_MOUSE_BUTTON_1);
    const bool shiftDown = in.is_key_down(GLFW_KEY_LEFT_SHIFT) || in.is_key_down(GLFW_KEY_RIGHT_SHIFT);

    // -------------------------------------------------------------------------
    // Compute forward/right/up from current yaw/pitch (used by both orbit & pan)
    const float cp = std::cos(pitch), sp = std::sin(pitch);
    const float cy = std::cos(yaw),   sy = std::sin(yaw);

    // forward dir points toward the target when yaw=0,pitch=0 -> +Z
    bx::Vec3 dir { cp * sy, sp, cp * cy };
    const bx::Vec3 worldUp { 0.0f, 1.0f, 0.0f };

    // right = dir × up; camUp = right × dir
    bx::Vec3 right = bx::normalize(bx::cross(dir, worldUp));
    bx::Vec3 camUp = bx::normalize(bx::cross(right, dir));

    // -------------------------------------------------------------------------
    // Input handling
    if (mmbDown && !shiftDown) {
        // ORBIT (rotate)
        math::Vec2 d = in.get_mouse_delta();     // pixels
        yaw   += d.x * rotateSpeed;              // drag left → yaw left
        pitch -= d.y * rotateSpeed;              // drag up → pitch up

        // clamp pitch to avoid flipping
        const float limit = bx::kPi * 0.5f - 0.001f;
        if (pitch >  limit) pitch =  limit;
        if (pitch < -limit) pitch = -limit;
    }
    else if (mmbDown && shiftDown) {
        // PAN (translate camera + target in view plane)
        // we move the *target* in view plane; eye will be rebuilt from yaw/pitch/dist.
        math::Vec2 dpx = in.get_mouse_delta();   // pixels
        // scale pan by distance so it feels consistent
        float panSpeed = panBase * dist;

        // Dragging mouse right should move the scene right → target moves along +right when dpx.x > 0.
        // Dragging up moves the scene up → target moves along +camUp when dpx.y < 0 (screen Y grows down),
        // so we subtract dpx.y.
        math::Vec3 delta =
            math::Vec3{ right.x * ( dpx.x * panSpeed),
                      right.y * ( dpx.x * panSpeed),
                      right.z * ( dpx.x * panSpeed) } +
            math::Vec3{ camUp.x  * (dpx.y * panSpeed),
                      camUp.y  * (dpx.y * panSpeed),
                      camUp.z  * (dpx.y * panSpeed) };

        target.x += delta.x;
        target.y += delta.y;
        target.z += delta.z;
    }

    // zoom with scroll wheel (positive = zoom in)
    math::Vec2 scroll = in.get_scroll_delta();       // per-frame scroll
    if (scroll.y != 0.0f) {
        dist *= std::pow(zoomStep, -scroll.y);       // exponential dolly
        if (dist < minDist) dist = minDist;
        if (dist > maxDist) dist = maxDist;
    }

    // Recompute eye from (target, yaw, pitch, dist)
    // (dir was computed earlier from yaw/pitch)
    bx::Vec3 at  { target.x, target.y, target.z };
    bx::Vec3 eye { at.x - dir.x * dist,
                   at.y - dir.y * dist,
                   at.z - dir.z * dist };

    // If the MMB was just released this frame, make the *screen center* the new target.
    // That is: project a ray from the eye through the center and take the point at distance 'dist'.
    // Equivalent to: target = eye + dir * dist.
    if (prevMMBDown && !mmbDown) {
        target.x = eye.x + dir.x * dist;
        target.y = eye.y + dir.y * dist;
        target.z = eye.z + dir.z * dist;

        // Rebuild 'at' (and eye stays where it is).
        at = bx::Vec3{ target.x, target.y, target.z };
    }
    prevMMBDown = mmbDown;

    // view / proj
    bx::mtxLookAt(outView, eye, at, worldUp);  // up = +Y
    bx::mtxProj(outProj, 60.0f, float(winW) / float(winH), 0.1f, 1000.0f,
                bgfx::getCaps()->homogeneousDepth);
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
    std::snprintf(buf, sizeof(buf), "FPS: %d   (%.2f ms)", (int)fpsSmooth, ms);
    bgfx::dbgTextPrintf(1, 1, 0x9f, buf);
}


float f(float x, float y){
    return x*x/20.0 - y*y/20.0;
}

math::Vec3 norm(float x, float y){
// clac the deivative here
    float fx = 2*x/20.0;
    float fy =2*y/20.0;

    return math::normalize(math::Vec3{
        -fx, -fy, 1
    });

}

// --------------------------------------
// Main functions
// --------------------------------------

// This method will init all relevant engine stuff so after it u needto show window and enter main loop
// Called once at the start of entry
void init_engine(void){
    // < --- window init settings --- > // 
    WindowSettings ws{};
    if (!glfwInit()) { throw std::runtime_error("GLFW init failed\n"); }
    // We don't want GLFW to create an OpenGL context. bgfx will own the GPU.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // make the window invisible until first frame executed.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    win = glfwCreateWindow(ws.window_width, ws.window_height, ws.window_title.c_str(), nullptr, nullptr);
    if (!win) { throw std::runtime_error("Window creation failed\n"); }
    
    GLFWimage images[1];
    images[0].width  =  ICON_WIDTH;
    images[0].height =  ICON_HEIGHT;
    images[0].pixels = icon_pixels;
    glfwSetWindowIcon(win, 1, images);
    // <--- end window init settings ---> // 
    
    // init bgfx
    init_bgfx();

    // Init engine stuff
    Input::input& input_manager = Input::input::get(win);
    Shader::shader& shader_instance = Shader::shader::Instance();
    TimeSystem::time::init();
}

// This method should update all managers
// Called each frame
void Update(void){
    running = !glfwWindowShouldClose(win);
    TimeSystem::time::update();
    Input::input::get(win).update();
    Shader::shader::Instance().update();
}

// This method called each frame for 
inline static void HandleResize(int& w, int& h){
    glfwGetFramebufferSize(win, &w, &h);
    if (w != (int)init.resolution.width || h != (int)init.resolution.height) {
        init.resolution.width  = (uint32_t)w;
        init.resolution.height = (uint32_t)h;
        bgfx::reset(init.resolution.width, init.resolution.height, init.resolution.reset);
    }
}

// This method should handle (get notified and invoke relevant callbacks) of events
// Called each frame
void HandleEvents(void){
    
}

void HandleNavigation(const int& w, const int& h){
    float view[16], proj[16];
    bx::Vec3 eye = update_orbit_camera_blender( w, h, view, proj);
    bgfx::setViewTransform(0, view, proj);  
}

// This method should handle all the rendering stuff
// Called each frame 
void PrepareForRender(void){
    bgfx::setViewRect(
        0, 
        0, 0, 
        (uint16_t)init.resolution.width, (uint16_t)init.resolution.height
    );
    bgfx::setViewClear(
        0, 
        BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 
        0x141414d9, 
        1.0f, 
        0
    );
    bgfx::touch(0);
}

void Render(void){


}

void FinishRender(void){
    bgfx::frame();
}

// This method cleans all initiated resources
// Called once the main loop finished and the program is about to shutdown
inline static void Cleanup(void){
    Input::input::destroy();
    Shader::shader::Instance().cleanup();
    bgfx::shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();
}

// Entry - main
// ----------------
int main(void)
{
    init_engine();

    draw_axis(); // test purpose

    // Prepare for main loop
    int w = init.resolution.width, h = init.resolution.height;
    // show the window
    glfwShowWindow(win);
    // Main loop
    while (running)
    {
        // Events
        HandleResize(w, h);
        HandleEvents();
        HandleNavigation(w, h);
        
        // Update
        Update();

        // Render
        PrepareForRender();
        Render();
        FinishRender();

    }

    Cleanup();
    return EXIT_SUCCESS;
}

// End
// -------------------------------------------------------------------------------

// static const std::string vert_name = "test";
// static const std::string frag_name = "test";

// static bgfx::UniformHandle u_lightPosIntensity;
// static bgfx::UniformHandle u_cameraPosShininess;
// static bgfx::UniformHandle u_albedo;
// static bgfx::UniformHandle u_TimeDeltaMouseXY;
///////////==========================

    // ----------- simple rendering -------
    // cube *light_source = new cube(0xFF10A7E8);
    // light_source->create_renderer("light", "light");
    // light_source->set_transform(Components::transform{
    //     math::Vec3{3,3,3}, 
    //     math::Vec3{0, 0, 0}, 
    //     math::Vec3{0.5,0.5,0.5}
    // });
    //plane pl{0xffffffff};
    //pl.create_renderer("plane", frag_name);
    //Components::transform trans = pl.get_transform();
    //pl.set_transform(Components::transform{math::Vec3{7.5, 0.1, -7.5}, trans.get_rotation(), math::Vec3{15}});


    // --------------------------------


        //math::Vec3 pos = light_source->get_transform().get_position();
       
        // light & material
        // const bx::Vec3 lightDir = bx::normalize(bx::Vec3(-1.0f, 0.0f, -1.0f)); // world-space
        // float lightDirColor[4]      = { pos.x, pos.y, pos.z,1.0f}; // w = intensity
        // float cameraPosShininess[4] = { eye.x, eye.y , eye.z, 256.0f };                // .w = shininess
        // float albedo[4]             = { 233.0f/256.0f,168.0f/256.0f,16.0f/256.0f };

        // rotate light sourcein circle with radius dis
        // Components::transform tr = light_source->get_transform();
        // float dis = 10;
        // tr.set_position( math::Vec3{0,7,0} + math::Vec3{dis*bx::cos(angle), 0, dis*bx::sin(angle)});
        // light_source->set_transform(tr);


        // math::Vec2 p = input_manager.get_mouse_pos();
        // float timeDeltaMouseXY[4] = {angle ,TimeSystem::time::delta_time, p.x, p.y};
        
        // bgfx::setUniform(u_lightPosIntensity ,     lightDirColor);
        // bgfx::setUniform(u_cameraPosShininess,cameraPosShininess);
        // bgfx::setUniform(u_albedo            ,            albedo);
        // bgfx::setUniform(u_TimeDeltaMouseXY  ,  timeDeltaMouseXY);