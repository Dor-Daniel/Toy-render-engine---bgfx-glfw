#pragma once
#include "../config.hpp"

namespace Input {

class input {
private:
    GLFWwindow* win{nullptr};

    // Key state
    std::array<uint8_t, GLFW_KEY_LAST + 1> key_now{};
    std::array<uint8_t, GLFW_KEY_LAST + 1> key_prev{};

    // Mouse state
    std::array<uint8_t, GLFW_MOUSE_BUTTON_LAST + 1> mouse_now{};
    std::array<uint8_t, GLFW_MOUSE_BUTTON_LAST + 1> mouse_prev{};

    math::Vec2 mouse_pos{0.0f, 0.0f};
    math::Vec2 mouse_prev_pos{0.0f, 0.0f};
    math::Vec2 mouse_delta{0.0f, 0.0f};
    math::Vec2 scroll_delta{0.0f, 0.0f};

    struct DragState {
        bool dragging{false};
        math::Vec2 start{0.0f, 0.0f};
    };
    std::array<DragState, GLFW_MOUSE_BUTTON_LAST + 1> drag{};

    // Clicks / double-clicks
    std::array<double, GLFW_MOUSE_BUTTON_LAST + 1> last_click_time{};
    std::array<int, GLFW_MOUSE_BUTTON_LAST + 1> click_count{};
    double double_click_window{0.25}; // seconds

    // config
    float drag_threshold{3.0f}; // pixels
    bool focused{true};

    // Singleton instance
    static input* instance;

private:
    explicit input(GLFWwindow* _win);

public:
    ~input() = default;

    // Get singleton instance (must be initialized once with a window)
    static input& get(GLFWwindow* _win = nullptr) {
        if (!instance) {
            if (_win == nullptr) {
                throw std::runtime_error("Input not initialized: pass a GLFWwindow* the first time.");
            }
            instance = new input(_win);
        }
        return *instance;
    }

    // Destroy singleton instance
    static void destroy() {
        delete instance;
        instance = nullptr;
    }

    // Call once per frame
    void update();

    // Keys
    bool is_key_down(int key) const;
    bool is_key_pressed(int key) const;
    bool is_key_released(int key) const;

    // Mouse
    bool is_mouse_down(int button = GLFW_MOUSE_BUTTON_LEFT) const;
    bool is_mouse_pressed(int button = GLFW_MOUSE_BUTTON_LEFT) const;
    bool is_mouse_released(int button = GLFW_MOUSE_BUTTON_LEFT) const;

    bool is_mouse_dragging(int button = GLFW_MOUSE_BUTTON_LEFT) const;
    math::Vec2 get_drag_start(int button = GLFW_MOUSE_BUTTON_LEFT) const;
    math::Vec2 get_drag_delta(int button = GLFW_MOUSE_BUTTON_LEFT) const;

    math::Vec2 get_mouse_pos()   const;
    math::Vec2 get_mouse_delta() const;
    math::Vec2 get_scroll_delta() const;

    int click_count_for(int button = GLFW_MOUSE_BUTTON_LEFT) const;

    // Cursor helpers
    void set_cursor_locked(bool lock);
    void set_cursor_visible(bool show);

private:
    // GLFW callbacks (static trampolines)
    static input* self(GLFWwindow* w) { return static_cast<input*>(glfwGetWindowUserPointer(w)); }
    static void key_cb(GLFWwindow* w, int key, int sc, int action, int mods);
    static void mouse_button_cb(GLFWwindow* w, int button, int action, int mods);
    static void cursor_pos_cb(GLFWwindow* w, double x, double y);
    static void scroll_cb(GLFWwindow* w, double xoff, double yoff);
    static void focus_cb(GLFWwindow* w, int focused);
};

} // namespace Input
