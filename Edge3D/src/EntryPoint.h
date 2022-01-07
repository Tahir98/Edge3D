#pragma once

#include "Application.h"
#include "Log.h"

int main() {

	Edge3D::Log::init();

	/*ENG_LOG_ERROR("Error message");
	ENG_LOG_WARN("Warn message");
	ENG_LOG_INFO("Info message");
	ENG_LOG_TRACE("Trace message");
	
	APP_LOG_ERROR("Error message");
	APP_LOG_WARN("Warn message");
	APP_LOG_INFO("Info message");
	APP_LOG_TRACE("Trace message");*/

	Edge3D::Application* app = Edge3D::createApplication();

	app->run();

	delete app;

	return 0;
}