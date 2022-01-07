#pragma once

#include "Edge3D.h"
#include "imgui.h"
#include <vector>
#include "Model/Terrain.h"

using namespace Edge3D;

class TerrainGenerator : public Scene {
private:
	GLFWwindow* window;

	PerspectiveCamera camera;
	SpherecalSkybox skybox;

	InputHandler handler;

	
	Sphere sphere;
	float xPrev = 0, yPrev = 0;
	bool first = true;
	float sensivity = 0.025f;

	Terrain terrain;
	

	Light light;
public:

	TerrainGenerator(GLFWwindow* window) :Scene("Terrain Generator"), window(window),skybox("assets/rooitou_park.jpg"), 
		camera(70,Application::aspectRatio,0.1f,1000){
		sphere.debugMode = true;

		terrain.debugMode = true;


		light.pos = { 0,-5,0 };
		light.ambient = { 0.5f,0.5f ,0.5f };
		light.diffuse = { 0.8f,0.8f ,0.8f };
		light.specular = { 1.0f,1.0f ,1.0f };
	}

	void onCreate() override {

	}

	void onUpdate(const float delta) override {
		input(delta);
		camera.update(delta);

		skybox.draw(camera);
		

		sphere.draw(camera);
		

		terrain.draw(camera,light);

		light.pos = sphere.getPosition();

		imguiDraw(delta);

	}


	void input(const float delta) {
		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			double x, y;
			glfwGetCursorPos(window, &x, &y);

			if (first) {
				xPrev = x;
				yPrev = y;
				first = false;
			}
			else {
				float diffX = x - xPrev;
				float diffY = y - yPrev;

				xPrev = x;
				yPrev = y;

				camera.rotY(-diffX * sensivity);
				camera.rotX(diffY * sensivity);
			}
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			first = true;
		}

		if (handler.getKeyState(GLFW_KEY_W)) {
			camera.translate2(0, 0, 3.0f * delta);
		}
		else if (handler.getKeyState(GLFW_KEY_S)) {
			camera.translate2(0, 0, -3.0f * delta);
		}

		if (handler.getKeyState(GLFW_KEY_A)) {
			camera.translate2(-3.0f * delta, 0, 0);
		}
		else if (handler.getKeyState(GLFW_KEY_D)) {
			camera.translate2(3.0f * delta, 0, 0);
		}

		if (handler.getKeyState(GLFW_KEY_SPACE)) {
			camera.translate(0, 3.0f * delta, 0);
		}
		else if (handler.getKeyState(GLFW_KEY_LEFT_SHIFT)) {
			camera.translate(0, -3.0f * delta, 0);
		}
	}

	void imguiDraw(float delta) {
		ImGui::Begin("Settings");


		ImGui::End();
	}

	void onSuspend() override {

	}

	void onResume() override {

	}

	void onDestroy() override {

	}

};