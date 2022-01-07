#pragma once
#include "GLFW/glfw3.h"
#include <vector>
#include <utility>
#include "InputHandler.h"


namespace Edge3D {
    std::vector<std::pair<int, bool>> InputHandler::keyboard;
    std::vector<std::pair<int, bool>> InputHandler::mouse;

    Vec2 InputHandler::mousPos;

    /*static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            if (key <= 93) {
                keyboard[key - 32].second = true;
            }
            else if (key >= 256 && key <= 348) {
                keyboard[65 + (key - 256)].second = true;
            }
            else {
                if (key == 96) keyboard[62].second = true;
                if (key == 161) keyboard[63].second = true;
                if (key == 162) keyboard[64].second = true;
            }
        }

        if (action == GLFW_RELEASE) {
            if (key <= 93) {
                keyboard[key - 32].second = false;
            }
            else if (key >= 256 && key <= 348) {
                keyboard[65 + (key - 256)].second = false;
            }
            else {
                if (key == 96) keyboard[62].second = false;
                if (key == 161) keyboard[63].second = false;
                if (key == 162) keyboard[64].second = false;
            }
        }
    }


    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS) {
            mouse[button].second = true;
        }if (action == GLFW_RELEASE) {
            mouse[button].second = false;
        }
    }*/

    InputHandler::InputHandler() {
        //glfwSetKeyCallback((GLFWwindow*)window, key_callback);
        //glfwSetMouseButtonCallback((GLFWwindow*)window, mouse_button_callback);

        init();
    }

    InputHandler::~InputHandler() {
        keyboard.clear();
        keyboard.shrink_to_fit();

        mouse.clear();
        mouse.shrink_to_fit();
    }


    void InputHandler::init() {
        if (keyboard.size() == 0) {
            for (int i = 32; i <= 93; i++) {
                keyboard.push_back(std::make_pair(i, false));
            }

            keyboard.push_back(std::make_pair(96, false));
            keyboard.push_back(std::make_pair(161, false));
            keyboard.push_back(std::make_pair(162, false));

            for (int i = 256; i <= 348; i++) {
                keyboard.push_back(std::make_pair(i, false));
            }
        }

        if (mouse.size() == 0) {
            for (int i = 0; i <= 7; i++) {
                mouse.push_back(std::make_pair(i, false));
            }
        }
    }

    bool InputHandler::getKeyState(int key) {
        if (key <= 93) {
            return keyboard[key - 32].second;
        }
        else if (key >= 256 && key <= 348) {
            return keyboard[65 + (key - 256)].second;
        }
        else {
            if (key == 96) return keyboard[62].second;
            if (key == 161) return keyboard[63].second;
            if (key == 162) return keyboard[64].second;
        }

        return false;
    }

    bool InputHandler::getMouseButtonState(int button) {
        return mouse[button].second;
    }

    Vec2 InputHandler::getMousePos() {
        return mousPos;
    }

}



