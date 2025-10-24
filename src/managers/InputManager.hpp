// #pragma once
// #include "../config.hpp"

// // This is a singelton class which handles the input

// enum class Key{
//     A = GLFW_KEY_A, B = GLFW_KEY_B, C = GLFW_KEY_C,D = GLFW_KEY_D, E = GLFW_KEY_E, 
//     H = GLFW_KEY_H, I = GLFW_KEY_I, J = GLFW_KEY_J ,K = GLFW_KEY_K, L = GLFW_KEY_L,
//     M = GLFW_KEY_M, N = GLFW_KEY_N, O = GLFW_KEY_O, P = GLFW_KEY_P, Q = GLFW_KEY_Q, 
//     R = GLFW_KEY_R, S = GLFW_KEY_S, T = GLFW_KEY_T, U = GLFW_KEY_U, V = GLFW_KEY_V, 
//     W = GLFW_KEY_W, X = GLFW_KEY_X, Y = GLFW_KEY_Y, Z = GLFW_KEY_Z,
//     NUM_0 = GLFW_KEY_0, NUM_1 = GLFW_KEY_1, NUM_2 = GLFW_KEY_2, NUM_3 = GLFW_KEY_3,
//     NUM_4 = GLFW_KEY_4, NUM_5 = GLFW_KEY_5, NUM_6 = GLFW_KEY_6, NUM_7 = GLFW_KEY_7,
//     NUM_8 = GLFW_KEY_8, NUM_9 = GLFW_KEY_9,
//     LEFT = GLFW_KEY_LEFT, RIGHT = GLFW_KEY_RIGHT, UP = GLFW_KEY_UP, DOWN = GLFW_KEY_DOWN,
//     RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT, LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT, 
//     ENTER = GLFW_KEY_ENTER, ESC = GLFW_KEY_ESCAPE
// };

// class InputManager
// {
// private:
//     static InputManager* instance;
//     explicit InputManager() = default;
//     ~InputManager() = default;
    

//     // Fields
//     // -----------
//     math::Vec2 mouse_position;
//     math::Vec2 mouse_delta;
//     math::Vec3 world_mouse_position;
//     math::Vec2 mouse_wheel_delta;
//     math::Vec2 mouse_wheel_position;


// public:

//     static InputManager * Instance(){
//         if(instance == nullptr) instance = new InputManager();
//         return instance;
//     }

//     void Init(GLFWwindow * window);
//     void Update();


//     // Api
//     // ---------
//     math::Vec2 GetMousescreenPosition();
//     math::Vec2 GetMouseScreenDelta();
//     math::Vec2 GetMouseWorldPosition();

// };

