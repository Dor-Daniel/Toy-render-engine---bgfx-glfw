#include "config.hpp"
#include "shader/shader.hpp"
#include "drawer/cube.hpp"
#include "time/time.hpp"
#include "components/transform.hpp"
#include "input/input.hpp"
#include "drawer/line.hpp"
#include "drawer/extras/axis.hpp"

static const std::string vert_name = "test";
static const std::string frag_name = "test";

struct WindowSettings{
    uint16_t window_width = 1280;
    uint16_t window_height = 720;
    std::string window_title = "Window";
};

void init_bgfx(GLFWwindow * win, bgfx::Init* init){

    // Fill platform data for bgfx
    bgfx::PlatformData pd{};
    void* ndt = nullptr;
    pd.nwh = glfwGetWin32Window(win);

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

void test(){
    // cube c{}; 
    // c.set_transform(Components::transform{
    //     math::Vec3{0, 0, 0}, 
    //     math::Vec3{0, bx::kPi / 2, bx::kPi / 6}, 
    //     math::Vec3{1, 1, 1}
    // });
    cube c2{}; 
    c2.set_transform(Components::transform{
        math::Vec3{0, 0, 0}, 
        math::Vec3{0, 0, 0}, 
        math::Vec3{1, 1, 1}
    });
    c2.create_renderer(vert_name, frag_name, Shader::line_state);
    // c.create_renderer(vert_name, frag_name);

    // //create a line object
    // line ln({-1, -1, -1}, {2.0f, 2, 2}, 0xffffffff, 0xff00ffff);

    // // (optional) apply a transform to both endpoints
    // Components::transform t;
    // t = Components::transform({0,0,0}, {0,0,0}, {1,1,1});
    // ln.set_transform(t);

    // // build renderer; IMPORTANT: ask for PT_LINES so bgfx draws as lines
    // ln.create_renderer("vs_flat", "fs_flat", Shader::line_state);

    axis ax(XYZ_AXIS);
    ax.set_transform(Components::transform({0,0,0}, {0,0,0}, {1,1,1}));
    ax.set_length(20.0f);                           // axes from -5..+5
    ax.set_axes_colors(0xff0000ff, 0xff00ff00, 0xffff0000); // X=red, Y=green, Z=blue

    ax.enable_floor(true);
    ax.set_floor_plane(XZ_AXIS);
    ax.set_floor_lines_count(50);                  // 10 lines each direction (thin grid)
    ax.set_floor_color(0xff5a5a5a);

    // IMPORTANT: submit as lines
    ax.create_renderer_lines("vs_flat", "fs_flat");  // wraps mesh::create_renderer(..., state|PT_LINES)
    // or directly:
    // ax.create_renderer("vs_flat", "fs_flat", Shader::default_state | BGFX_STATE_PT_LINES);

}

static void update_orbit_camera_blender(Input::input& in, int winW, int winH,
                                        float* outView /*16*/, float* outProj /*16*/)
{
    // persistent camera state (orbit around target)
    static float yaw   = 0.0f;            // radians, around +Y
    static float pitch = 0.0f;            // radians, around +X
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
    bx::Vec3 at  { target.x, target.y, target.z };
    bx::Vec3 dir { cp*sy, sp, cp*cy };              // +Z when yaw=0,pitch=0
    bx::Vec3 eye { at.x - dir.x*dist, at.y - dir.y*dist, at.z - dir.z*dist };

    // view / proj
    bx::mtxLookAt(outView, eye, at, bx::Vec3{0.0f, 1.0f, 0.0f});  // up = +Y  
    bx::mtxProj(outProj, 60.0f, float(winW)/float(winH), 0.1f, 1000.0f,
                bgfx::getCaps()->homogeneousDepth);    
}


int main()
{
    // < --- window init settings --- > // 
    WindowSettings ws{};
    if (!glfwInit()) { std::fprintf(stderr, "GLFW init failed\n"); return 1; }
    // We don't want GLFW to create an OpenGL context. bgfx will own the GPU.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // make the window invisible until first frame executed.
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(ws.window_width, ws.window_height, ws.window_title.c_str(), nullptr, nullptr);
    if (!win) { std::fprintf(stderr, "Window creation failed\n"); return 1; }
    // < --- end window init settings --- > // 

    // create managers 
    Input::input input_manager(win);
    auto& inst = Shader::shader::Instance();
    Time::time::init();

    // init bgfx
    bgfx::Init init{};
    init_bgfx(win, &init);
    
    static bgfx::UniformHandle u_resolution =
        bgfx::createUniform("u_resolution", bgfx::UniformType::Vec4);

    test();

    int w = ws.window_width, h = ws.window_height;
    // show the window
    glfwShowWindow(win);

    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        Time::time::update();
        input_manager.update();

        // Resize handling
        glfwGetFramebufferSize(win, &w, &h);
        if (w != (int)init.resolution.width || h != (int)init.resolution.height) {
            init.resolution.width  = (uint32_t)w;
            init.resolution.height = (uint32_t)h;
            bgfx::reset(init.resolution.width, init.resolution.height, init.resolution.reset);
        }

        // View 0 = full screen clear
        bgfx::setViewRect(0, 0, 0, (uint16_t)init.resolution.width, (uint16_t)init.resolution.height);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x141414d9, 1.0f, 0);

        // float view[16]; bx::mtxLookAt(view, bx::Vec3{0.0f, 0.0f, -10}, bx::Vec3{0.0f, 0, 0.0f});
        // float proj[16]; bx::mtxProj(proj, 60.0f, float(w)/float(h), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        // bgfx::setViewTransform(0, view, proj);

        bgfx::touch(0); // ensure view is valid even if we submit nothing

        float res[4] = { float(w), float(h), 0.0f, 0.0f }; // you already track w,h
        bgfx::setUniform(u_resolution, res);
        struct V { float x,y,z; uint32_t abgr; };

        float view[16], proj[16];
        update_orbit_camera_blender(input_manager, w, h, view, proj);
        bgfx::setViewTransform(0, view, proj);   // same as before  :contentReference[oaicite:4]{index=4}


        inst.update();
        bgfx::frame();
    }

    if (bgfx::isValid(u_resolution)) bgfx::destroy(u_resolution);
    inst.cleanup();
    bgfx::shutdown();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
