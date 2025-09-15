#include "input.hpp"

namespace Input {

input::input(GLFWwindow* _win) : win{_win} {
    glfwSetWindowUserPointer(win, this);

    // Seed current states from GLFW so we don't get a bogus first frame.
    for (int k = 0; k <= GLFW_KEY_LAST; ++k)
        key_now[k] = glfwGetKey(win, k) == GLFW_PRESS;

    for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b)
        mouse_now[b] = glfwGetMouseButton(win, b) == GLFW_PRESS;

    double x = 0.0, y = 0.0;
    glfwGetCursorPos(win, &x, &y);
    mouse_pos = mouse_prev_pos = { static_cast<float>(x), static_cast<float>(y) };

    // Hook callbacks
    glfwSetKeyCallback(win, key_cb);
    glfwSetMouseButtonCallback(win, mouse_button_cb);
    glfwSetCursorPosCallback(win, cursor_pos_cb);
    glfwSetScrollCallback(win, scroll_cb);
    glfwSetWindowFocusCallback(win, focus_cb);
}

void input::update() {
    
    // Copy previous states
    key_prev   = key_now;
    mouse_prev = mouse_now;
    mouse_prev_pos = mouse_pos;
    
    // Reset per-frame deltas; callbacks will accumulate scroll during the frame
    scroll_delta = {0.0f, 0.0f};
    mouse_delta  = {0.0f, 0.0f}; // we'll compute below
    
    // Poll OS events first
    glfwPollEvents();
    // Compute mouse delta from last frame
    double x = 0.0, y = 0.0;
    glfwGetCursorPos(win, &x, &y);
    mouse_pos = { static_cast<float>(x), static_cast<float>(y) };
    mouse_delta = { mouse_pos.x - mouse_prev_pos.x, mouse_pos.y - mouse_prev_pos.y };

    // Update drag states (lazy threshold check)
    for (int b = 0; b <= GLFW_MOUSE_BUTTON_LAST; ++b) {
        if (mouse_now[b]) {
            // if just pressed, the start was set in the callback; keep checking threshold
            float dx = mouse_pos.x - drag[b].start.x;
            float dy = mouse_pos.y - drag[b].start.y;
            if (!drag[b].dragging && (dx*dx + dy*dy) >= (drag_threshold * drag_threshold))
                drag[b].dragging = true;
        } else {
            drag[b].dragging = false; // end drag on release
        }
    }
}

/*** Keys ***/
bool input::is_key_down(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return key_now[key];
}
bool input::is_key_pressed(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return key_now[key] && !key_prev[key];
}
bool input::is_key_released(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !key_now[key] && key_prev[key];
}

/*** Mouse ***/
bool input::is_mouse_down(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return mouse_now[button];
}
bool input::is_mouse_pressed(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return mouse_now[button] && !mouse_prev[button];
}
bool input::is_mouse_released(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return !mouse_now[button] && mouse_prev[button];
}

bool input::is_mouse_dragging(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return mouse_now[button] && drag[button].dragging;
}
math::Vec2 input::get_drag_start(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return {0.0f, 0.0f};
    return drag[button].start;
}
math::Vec2 input::get_drag_delta(int button) const {
    if (!is_mouse_down(button)) return {0.0f, 0.0f};
    return { mouse_pos.x - drag[button].start.x, mouse_pos.y - drag[button].start.y };
}

math::Vec2 input::get_mouse_pos() const   { return mouse_pos; }
math::Vec2 input::get_mouse_delta() const { return mouse_delta; }
math::Vec2 input::get_scroll_delta() const { return scroll_delta; }

int input::click_count_for(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return 0;
    return click_count[button];
}

/*** Cursor helpers ***/
void input::set_cursor_locked(bool lock) {
    glfwSetInputMode(win, GLFW_CURSOR, lock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void input::set_cursor_visible(bool show) {
    // Only change visibility if not already disabled (locked)
    if (glfwGetInputMode(win, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
        glfwSetInputMode(win, GLFW_CURSOR, show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

/*** Callbacks ***/
void input::key_cb(GLFWwindow* w, int key, int, int action, int) {
    if (key < 0 || key > GLFW_KEY_LAST) return;
    if (auto* s = self(w)) s->key_now[key] = (action != GLFW_RELEASE);
}
void input::mouse_button_cb(GLFWwindow* w, int button, int action, int) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;
    if (auto* s = self(w)) {
        s->mouse_now[button] = (action != GLFW_RELEASE);
        if (action == GLFW_PRESS) {
            // start potential drag here
            s->drag[button].start = s->mouse_pos;
            s->drag[button].dragging = false;

            // click / double-click
            double t = glfwGetTime();
            if (t - s->last_click_time[button] <= s->double_click_window)
                s->click_count[button] += 1;
            else
                s->click_count[button] = 1;
            s->last_click_time[button] = t;
        }
    }
}
void input::cursor_pos_cb(GLFWwindow* w, double x, double y) {
    if (auto* s = self(w)) {
        s->mouse_pos = { static_cast<float>(x), static_cast<float>(y) };
        // mouse_delta is recomputed each frame in update()
    }
}
void input::scroll_cb(GLFWwindow* w, double xoff, double yoff) {
    if (auto* s = self(w)) {
        s->scroll_delta.x += static_cast<float>(xoff);
        s->scroll_delta.y += static_cast<float>(yoff);
    }
}
void input::focus_cb(GLFWwindow* w, int foc) {
    if (auto* s = self(w)) {
        s->focused = (foc == GLFW_TRUE);
        if (!s->focused) {
            // Clear current state to avoid "stuck" inputs on alt-tab
            std::fill(s->key_now.begin(),   s->key_now.end(),   0);
            std::fill(s->mouse_now.begin(), s->mouse_now.end(), 0);
            for (auto& d : s->drag) d.dragging = false;
        }
    }
}

} // namespace Input
