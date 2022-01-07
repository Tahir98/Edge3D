#include "Log.h"

namespace Edge3D {

	
	std::shared_ptr<spdlog::logger> Log::coreLogger;
	std::shared_ptr<spdlog::logger> Log::appLogger;
	
	void Log::init() {
	
		coreLogger = spdlog::stdout_color_mt("Edge3D");
		coreLogger->set_pattern("%^[%H:%M:%S] [%n] %v%$");
		coreLogger->set_level(spdlog::level::trace);
		
		appLogger = spdlog::stdout_color_mt("Application");
		appLogger->set_pattern("%^[%H:%M:%S] [%n] %v%$");
		appLogger->set_level(spdlog::level::trace);
	}
}


