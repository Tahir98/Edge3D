#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


class ImGuiLayer {
public:
    static void init(GLFWwindow* window);
    static void begin();
    static void end();
    static void terminate();
};