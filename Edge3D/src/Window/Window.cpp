#include "Window.h"
#include "Log.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/WindowEvent.h"
#include <assert.h>

namespace Edge3D {
	Window::Window(std::string name, const int width, const int height, const bool vsync, const bool fullscreen, bool isImGuiFullscreen){
		prop.name = name;
		prop.width = width;
		prop.height = height;
		prop.aspectRatio = (float)prop.height / (float)prop.width;
		prop.vsync = vsync;
		prop.fullscreen = fullscreen;

		init(isImGuiFullscreen);
	}

	void Window::init(bool isImGuiFullscreen){
		if (glfwInit() == GLFW_FALSE) {
			ENG_LOG_ERROR("Failed initialize GLFW");
			exit(-1);
		}

		//glfwWindowHint(GLFW_VERSION_MAJOR, 4);
		//glfwWindowHint(GLFW_VERSION_MINOR, 3);
		//glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
		//
		//window = glfwCreateWindow(prop.width, prop.height, prop.name.c_str(), nullptr, nullptr);
		if (prop.fullscreen) {
			window = glfwCreateWindow(prop.width, prop.height, prop.name.c_str(), glfwGetPrimaryMonitor(), nullptr);
		}
		else {
			window = glfwCreateWindow(prop.width, prop.height, prop.name.c_str(), nullptr, nullptr);
		}
		
		if (window == nullptr) {
			ENG_LOG_ERROR("Failed to create window");
			
			exit(-1);
		}

		glfwSetWindowUserPointer(window,(void*)&prop);
		glfwMakeContextCurrent(window);

		gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);


		glfwSwapInterval((int)prop.vsync);

		ENG_LOG_INFO("Window created, Name: {0}, Width: {1}, Height: {2}",prop.name,prop.width,prop.height);

		running = true;


		glfwGetWindowPos(window,&prop.x,&prop.y);
		prop.log = false;

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (action == GLFW_PRESS) {
				KeyPressedEvent event(key);
				if(((WindowProperties*)glfwGetWindowUserPointer(window))->log)
					ENG_LOG_INFO(event.getMessage().c_str());
			}
			else if (action == GLFW_RELEASE) {
				KeyReleasedEvent event(key);
				if (((WindowProperties*)glfwGetWindowUserPointer(window))->log)
					ENG_LOG_INFO(event.getMessage().c_str());
			}
			else if (action == GLFW_REPEAT) {
				KeyRepeatedEvent event(key);
				if (((WindowProperties*)glfwGetWindowUserPointer(window))->log)
					ENG_LOG_INFO(event.getMessage().c_str());
			}		
		});

		glfwSetWindowPosCallback(window, [](GLFWwindow* window, int xpos, int ypos) {
			WindowMovedEvent event(xpos,ypos);
			WindowProperties* prop = (WindowProperties*)glfwGetWindowUserPointer(window);
			if (prop->log)
				ENG_LOG_INFO(event.getMessage().c_str());

			prop->x = xpos;
			prop->y = ypos;
			});

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			WindowResizedEvent event(width, height);
			WindowProperties* prop = (WindowProperties*)glfwGetWindowUserPointer(window);
			if (prop->log)
				ENG_LOG_INFO(event.getMessage().c_str());

			prop->width = width;
			prop->height = height;
			prop->aspectRatio = (float)prop->height / (float)prop->width;
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			WindowClosedEvent event;
			WindowProperties* prop = (WindowProperties*)glfwGetWindowUserPointer(window);
			if (prop->log)
				ENG_LOG_INFO(event.getMessage().c_str());
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			if (action == GLFW_PRESS) {
				MouseButtonPressedEvent event(button);
				if (((WindowProperties*)glfwGetWindowUserPointer(window))->log)
					ENG_LOG_INFO(event.getMessage().c_str());
			}
			else if (action == GLFW_RELEASE) {
				MouseButtonReleasedEvent event(button);
 				if (((WindowProperties*)glfwGetWindowUserPointer(window))->log)
					ENG_LOG_INFO(event.getMessage().c_str());
			}
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			MouseMovedEvent event((float)xpos,(float)ypos);
			WindowProperties* prop = (WindowProperties*)glfwGetWindowUserPointer(window);
			if (prop->log)
				ENG_LOG_INFO(event.getMessage().c_str());
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			MouseScrolledEvent event((float)xoffset, (float)yoffset);
			WindowProperties* prop = (WindowProperties*)glfwGetWindowUserPointer(window);
			if (prop->log)
				ENG_LOG_INFO(event.getMessage().c_str());
			});

		InputHandler::init();
		if (!isImGuiFullscreen) {
			ImGuiLayer::init(window,false);
		}
		else {
			ImGuiLayer::init(window, true);
		}
		

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_PROGRAM_POINT_SIZE);
		glDepthFunc(GL_LEQUAL);
	}

	Window::~Window(){
		ImGuiLayer::terminate();
	}

	void Window::update(){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

		

		if (previous == 0) {
			previous = now = (float)glfwGetTime();
		}
		else {
			previous = now;
			now = (float)glfwGetTime();
		}

		delta = now - previous;

		if (glfwWindowShouldClose(window))
			running = false;

		glfwPollEvents();
		ImGuiLayer::begin();
	}

	void Window::postEvents(){
		ImGuiLayer::end();
		glfwSwapBuffers(window);
	}

	void Window::shutdown() {
		glfwDestroyWindow(window);
		ENG_LOG_INFO("Window destroyed, Name: {0}, Width: {1}, Height: {2}", prop.name, prop.width, prop.height);
		glfwTerminate();
		ENG_LOG_INFO("GLFW terminated");
	}

	bool Window::isRunning() {
		return running;
	}

	void Window::setWidth(const int width){
		if (width > 0) {
			prop.width = width;
			prop.aspectRatio = (float)prop.height / (float)prop.width;
			glfwSetWindowSize(window,prop.width,prop.height);
			glViewport(0,0,prop.width,prop.height);
		}
		else {
			ENG_LOG_WARN("Width must be bigger 0");
		}
	}

	int Window::getWidth() {
		return prop.width;
	}

	void Window::setHeight(const int height){
		if (height > 0) {
			prop.height = height;
			prop.aspectRatio = (float)prop.height / (float)prop.width;
			glfwSetWindowSize(window, prop.width, prop.height);
			glViewport(0, 0, prop.width, prop.height);
		}
		else {
			ENG_LOG_WARN("Width must be bigger 0");
		}
	}

	int Window::getHeight() {
		return prop.height;
	}

	float Window::getAspectRatio() {
		return prop.aspectRatio;
	}

	void Window::setVsync(const bool vsync) {
		prop.vsync = vsync;
		glfwSwapInterval(prop.vsync);
	}

	bool Window::getVsync() {
		return prop.vsync;
	}

	float Window::getDelta(){
		return delta;
	}
	
	GLFWwindow* Window::getGLFWWindowPointer() {
		return window;
	}

}