#pragma once 
#include "Edge3D.h"

class PBRScene : public Edge3D::Scene {
private:
	struct BasicLigt {
		Vec3 pos;
		Vec3 color;
	};

	struct Ball {
		Vec3 pos;
		Vec3 albedo;
		float metallic;
		float roughness;
		float ao = 1.0f;
	};

	Edge3D::Window& window;

	Edge3D::PerspectiveCamera camera;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::FrameBuffer* fb;

	bool first = true;
	Edge3D::InputHandler handler;
	Vec2 prev, now;
	float cameraSensivity = 0.020f;
	float cameraSpeed = 10;

	Edge3D::SphereMesh smesh;

	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::VertexArray va;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::IndexBuffer* lib = nullptr;

	Edge3D::Shader* shader = nullptr;
	Edge3D::Shader* lshader = nullptr;

	BasicLigt lights[4];
	std::vector<Ball> balls;

	Edge3D::Texture* albedo = nullptr;
	Edge3D::Texture* roughness = nullptr;
	Edge3D::Texture* metallic = nullptr;
	Edge3D::Texture* ao = nullptr;

public:
	PBRScene(Edge3D::Window& window, std::string name) : window(window), Scene(name), camera(70, 9.0f / 16, 0.1f, 1000), skybox("assets/forest_slope.jpg") {
		camera.setPosition(0, 5, 0);
		fb = new Edge3D::FrameBuffer(600, 600);

		smesh.setSegment(20,20);
		smesh.includeTexCoords = true;
		smesh.calculate();

		vb = new Edge3D::VertexBuffer((float*)smesh.getVertices(), smesh.getVerticesSize(), GL_STATIC_DRAW);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		ib = new Edge3D::IndexBuffer(smesh.getIndices(), smesh.getIndicesSize(), GL_STATIC_DRAW);
		lib = new Edge3D::IndexBuffer(smesh.getLineIndices(), smesh.getLineIndicesSize(), GL_STATIC_DRAW);

		shader = new Edge3D::Shader("Shaders/PBR.shader");
		lshader = new Edge3D::Shader("Shaders/wireframe.shader");

		lights[0].pos = { 3,1,0 };
		lights[0].color = { 1,1,1 };

		lights[1].pos = { 3,0,0 };
		lights[1].color = { 1,1,1 };

		lights[2].pos = { 3,-1,0 };
		lights[2].color = { 1,1,1 };

		lights[3].pos = { 3,-2,0 };
		lights[3].color = { 1,1,1 };


		float cornerZ = -5;
		float cornerY = -5;
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				Ball ball;
				ball.pos = { 0,cornerY + 2.0f * i, cornerZ + 2.0f * j };
				ball.albedo = { 1,0,0 };
				ball.metallic = 0.8f;
				ball.roughness = 0.2f;
				ball.ao = 1;

				balls.push_back(ball);
			}
		}

		albedo = new Edge3D::Texture("assets/texture/rusty/albedo.png");
		metallic = new Edge3D::Texture("assets/texture/rusty/metallic.png");
		roughness = new Edge3D::Texture("assets/texture/rusty/roughness.png");
	}

	~PBRScene() {
		delete vb;
		delete ib;
		delete lib;
		delete shader;
		delete lshader;

		delete fb;
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

		glEnable(GL_DEPTH_TEST);

		va.bind();
		shader->bind();
		ib->bind();

		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());


		shader->setUniform3f("light[0].pos", lights[0].pos.x, lights[0].pos.y, lights[0].pos.z);
		shader->setUniform3f("light[0].color", lights[0].color.x, lights[0].color.y, lights[0].color.z);

		shader->setUniform3f("light[1].pos", lights[1].pos.x, lights[1].pos.y, lights[1].pos.z);
		shader->setUniform3f("light[1].color", lights[1].color.x, lights[1].color.y, lights[1].color.z);

		shader->setUniform3f("light[2].pos", lights[2].pos.x, lights[2].pos.y, lights[2].pos.z);
		shader->setUniform3f("light[2].color", lights[2].color.x, lights[2].color.y, lights[2].color.z);

		shader->setUniform3f("light[3].pos", lights[3].pos.x, lights[3].pos.y, lights[3].pos.z);
		shader->setUniform3f("light[3].color", lights[3].color.x, lights[3].color.y, lights[3].color.z);

		albedo->bind(0);
		shader->setUniform1i("albedoMap", 0);

		metallic->bind(1);
		shader->setUniform1i("metallicMap", 1);

		roughness->bind(2);
		shader->setUniform1i("roughnessMap", 2);

		for (int i = 0; i < balls.size(); i++) {
			Ball& ball = balls[i];
			Matrix4x4 model = Math::translate(ball.pos);
			shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
			shader->setUniform1f("ao", ball.ao);

			glDrawElements(GL_TRIANGLES, smesh.getIndicesSize() / 4, GL_UNSIGNED_INT, 0);
		}
	

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
			glfwSetInputMode(window.getGLFWWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("FPS: %.2f", 1.0f / delta);

		ImGui::End();
	}

};