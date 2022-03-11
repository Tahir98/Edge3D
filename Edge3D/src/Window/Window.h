#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <string>

namespace Edge3D {

	struct WindowProperties {
		std::string name;
		int width, height;
		float aspectRatio;
		int x, y;
		bool vsync;
		bool fullscreen;
		bool log;
	};

	class Window {
	private:
		GLFWwindow* window = nullptr;
		WindowProperties prop;
		
		bool running = false;

		float previous = 0, now = 0;
		float delta;
		bool logMessages = true;
	public:
		Window(std::string name ,const int width,const int height,const bool vsync,const bool fullscreen, bool isImGuiFullscreen);
		~Window();

		void update();
		void postEvents();
		
		void shutdown();

		float getDelta();
		bool isRunning();

		void setWidth(const int width);
		int getWidth();

		void setHeight(const int height);
		int getHeight();

		float getAspectRatio();

		void setVsync(const bool vsync);
		bool getVsync();

		GLFWwindow* getGLFWWindowPointer();
		
	private:
		void init(bool isImGuiFullscreen);
	};

}