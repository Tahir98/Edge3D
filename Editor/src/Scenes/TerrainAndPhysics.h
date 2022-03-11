#pragma once

#include "Edge3D.h"
#include "Procedural/Terrain.h"

class T_P : public Edge3D::Scene{
private:
	Edge3D::Window& window;

	Edge3D::PerspectiveCamera camera;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::FrameBuffer* fb;

	bool first = true;
	Edge3D::InputHandler handler;
	Vec2 prev, now;
	float cameraSensivity = 0.020f;
	float cameraSpeed = 10;

	Terrain terrain;
	DirectionalLight light;

	int width = 100, height = 100;

public:
	T_P(Edge3D::Window& window, std::string name) : window(window), Scene(name), camera(70, 9.0f/16,0.1f,1000), skybox("assets/forest_slope.jpg"), terrain(100,100,false){
		terrain.generate();
		//terrain.setTexture("assets/texture/fabric/fabric_col_1_2k.png");

		light.direction = { 0,-1,0 };
		light.ambient = { 0.3f,0.3f,0.3f };
		light.diffuse = { 0.5f,0.5f,0.5f };
		light.specular = { 0.7f,0.7f,0.7f };
		light.intensity = 1.0f;

		camera.setPosition(0, 5, 0);
		fb = new Edge3D::FrameBuffer(600, 600);
	}

	~T_P() {

	}

	void onCreate() {

	}

	void onUpdate(float delta) {
		input(delta);

		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);

		camera.update(delta);
		skybox.draw(camera);

		//glEnable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);

		terrain.draw(camera,light);

		fb->unbind();

		ImGui::Begin("Scene");
		ImVec2 panelSize = ImGui::GetContentRegionAvail();
		if (panelSize.x != fb->getWidth() || panelSize.y != fb->getHeight()) {
			fb->resize(panelSize.x, panelSize.y);
		}
		ImGui::Image((ImTextureID)fb->getTextureID(), panelSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}

	void onSuspend() {
		APP_LOG_INFO("Scene onSuspend function called, id: {0}", id);

	}

	void onResume() {
		APP_LOG_INFO("Scene onResume function called, id: {0}", id);
	}

	void onDestroy() {
		APP_LOG_INFO("Scene onDestroy function called, id: {0}", id);
	}

	void input(float delta) {
		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && first) {
			first = false;
			glfwSetInputMode(window.getGLFWWindowPointer(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);

			prev = now = handler.getMousePos();
		}
		else if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
			prev = now;
			now = handler.getMousePos();

			camera.rotX((prev.y - now.y) * cameraSensivity);
			camera.rotY((now.x - prev.x) * cameraSensivity);
		}
		else if (!handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !first) {
			first = true;
			glfwSetInputMode(window.getGLFWWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (handler.getKeyState(GLFW_KEY_W)) {
			camera.translate2(0, 0, cameraSpeed * delta);
		}
		else if (handler.getKeyState(GLFW_KEY_S)) {
			camera.translate2(0, 0, -cameraSpeed * delta);
		}

		if (handler.getKeyState(GLFW_KEY_A)) {
			camera.translate2(-cameraSpeed * delta, 0, 0);
		}
		else if (handler.getKeyState(GLFW_KEY_D)) {
			camera.translate2(cameraSpeed * delta, 0, 0);
		}

		if (handler.getKeyState(GLFW_KEY_SPACE)) {
			camera.translate(0, cameraSpeed * delta, 0);
		}
		else if (handler.getKeyState(GLFW_KEY_LEFT_SHIFT)) {
			camera.translate(0, -cameraSpeed * delta, 0);
		}

		ImGui::Begin("Settings");

		ImGui::Checkbox("Wireframe", &terrain.wireframe);
		ImGui::SliderFloat("Camera speed", &cameraSpeed, 1, 200);
		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Light properties");
		ImGui::SliderFloat3("direction", (float*)&light.direction, -1, 1);
		ImGui::SliderFloat3("ambient", (float*)&light.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&light.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&light.specular, 0, 1);
		ImGui::SliderFloat("intensity", &light.intensity, 0, 5);

		Material& material = terrain.getMaterial();

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Terrain material propperties");
		ImGui::SliderFloat3("Ambient", (float*)&material.ambient, 0, 1);
		ImGui::SliderFloat3("Diffuse", (float*)&material.diffuse, 0, 1);
		ImGui::SliderFloat3("Specular", (float*)&material.specular, 0, 1);
		ImGui::SliderFloat("shininess", &material.shininess, 0, 200);

		ImGui::NewLine();
		ImGui::Separator();

		ImGui::Text("Terrain properties");
		ImGui::SliderInt("Width", &width, 2, 1000);
		ImGui::SliderInt("Height", &height, 2, 1000);

		if (ImGui::Button("Generate", ImVec2(100, 40))) {
			terrain.setSize(width, height);
			terrain.generate();
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("FPS: %.2f", 1.0f / delta);

		ImGui::End();
	}

};