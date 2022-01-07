#pragma once

namespace Edge3D {
	class Application {
	public:
		Application();
		virtual ~Application();

		virtual void run();
	};

	extern inline Application* createApplication();
}

