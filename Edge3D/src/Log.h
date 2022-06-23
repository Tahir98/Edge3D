#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>

namespace Edge3D {
	class Log {
	public:
		static void init();
		static std::shared_ptr<spdlog::logger> coreLogger;
		static std::shared_ptr<spdlog::logger> appLogger;
	};
}

#define ENG_LOG_WARN(...)   Edge3D::Log::coreLogger->warn(__VA_ARGS__)
#define ENG_LOG_ERROR(...)  Edge3D::Log::coreLogger->error(__VA_ARGS__)
#define ENG_LOG_INFO(...)   Edge3D::Log::coreLogger->info(__VA_ARGS__)
#define ENG_LOG_TRACE(...)  Edge3D::Log::coreLogger->trace(__VA_ARGS__)


#define APP_LOG_WARN(...)   Edge3D::Log::appLogger->warn(__VA_ARGS__)
#define APP_LOG_ERROR(...)  Edge3D::Log::appLogger->error(__VA_ARGS__)
#define APP_LOG_INFO(...)   Edge3D::Log::appLogger->info(__VA_ARGS__)
#define APP_LOG_TRACE(...)  Edge3D::Log::appLogger->trace(__VA_ARGS__)