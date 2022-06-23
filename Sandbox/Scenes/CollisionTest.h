#pragma once

#include <Edge3D.h>
#include<vector>



class CollisionTest : public Edge3D::Scene {
private:
	struct Cube {
		Vec3 pos;
		Vec3 size;
		Vec3 color;
	};

	Edge3D::Window& window;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::PerspectiveCamera camera;
	Edge3D::InputHandler handler;
	Vec2 now, prev;
	bool isClicked = false;
	float sensivity = 0.045f;
	float cameraSpeed = 10.0f;

	Edge3D::FrameBuffer* fb = nullptr;

	Edge3D::Shader* shader = nullptr;
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::VertexArray va;
	Edge3D::IndexBuffer* ib = nullptr;

	Cube c1,c2;
public:
	CollisionTest(Edge3D::Window& window, std::string name) : window(window), Scene(name), skybox("assets/whale_skeleton.jpg"), camera(70, 9.0f / 16.0f, 0.1f, 500) {
		fb = new Edge3D::FrameBuffer(600, 600);

		float vertices[] = {
			0,0,0,	0,0,0,	 //0
			0,1,0,	0,1,0,	 //1
			1,1,0,	1,1,0,	 //2
			1,0,0,	1,0,0,	 //3
			1,0,1,	1,0,1,	 //4
			1,1,1,	1,1,1,	 //5
			0,1,1,	0,1,1,	 //6
			0,0,1,	0,0,1	 //7
		};

		uint32_t indices[] = {
			0,1,2,	0,2,3,
			3,2,5,	3,5,4,
			4,5,6,	4,6,7,
			7,6,1,	7,1,0,
			1,6,5,	1,5,2,
			7,0,3,	7,3,4
		};

		vb = new Edge3D::VertexBuffer(vertices,sizeof(vertices),GL_STATIC_DRAW);
		va.bind();
		vb->bind();
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		ib = new Edge3D::IndexBuffer(indices, sizeof(indices), GL_STATIC_DRAW);

		shader = new Edge3D::Shader("Shaders/collisiontest.shader");

		c1.pos = { -1,0,-1 };
		c1.size = { 1,1,1 };
		c1.color = { 1,1,1 };

		c2.pos = { 1,0,1 };
		c2.size = { 1,1,1 };
		c2.color = { 1,1,1 };
	}

	~CollisionTest() {
		delete fb;
	}

	void onCreate() {
		APP_LOG_INFO("Scene onCreate function called, id: {0}", id);
	}

	void onUpdate(float delta) {
		input(delta);
		camera.update(delta);

		if (collitionTest(c1, c2))
			c1.color = { 0,0,0 };
		else
			c1.color = { 1,1,1 };

		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);
		glViewport(0, 0, fb->getWidth(), fb->getHeight());
		glEnable(GL_CULL_FACE);
		skybox.draw(camera);

		shader->bind();
		va.bind();
		ib->bind();

		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		shader->setUniform3f("position", c1.pos.x, c1.pos.y, c1.pos.z);
		shader->setUniform3f("u_color", c1.color.x, c1.color.y, c1.color.z);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		shader->setUniform3f("position", c2.pos.x, c2.pos.y, c2.pos.z);
		shader->setUniform3f("u_color", c2.color.x, c2.color.y, c2.color.z);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		fb->unbind();

		ImGui::Begin("Demo");

		ImVec2 PanelSize = ImGui::GetContentRegionAvail();
		Vec2 fb_size = { (float)fb->getWidth(),(float)fb->getHeight() };

		if (PanelSize.x != fb_size.x || PanelSize.y != fb_size.y) {
			fb->resize(PanelSize.x, PanelSize.y);
			ImVec2 size = ImGui::GetWindowSize();
			camera.setAspectRatio((float)PanelSize.y / (float)PanelSize.x);
		}

		ImGui::Image((ImTextureID)fb->getTextureID(), PanelSize, ImVec2(0, 1), ImVec2(1, 0));

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

	void check() {
		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL Error: error code = {0}", error);
		}
	}

	bool collitionTest(const Cube a, const Cube b) {
		return (a.pos.x <= b.pos.x + b.size.x && a.pos.x + a.size.x >= b.pos.x) &&
			(a.pos.y <= b.pos.y + b.size.y && a.pos.y + a.size.y >= b.pos.y) &&
			(a.pos.z <= b.pos.z + b.size.z && a.pos.z + a.size.z >= b.pos.z);
	}

	void input(float delta) {
		ImGui::Begin("Properties");

		ImGui::End();

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !isClicked) {
			prev = now = handler.getMousePos();
			isClicked = true;

			glfwSetInputMode(window.getGLFWWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
			prev = now;
			now = handler.getMousePos();
		}
		else {
			isClicked = false;
			glfwSetInputMode(window.getGLFWWindowPointer(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (isClicked) {
			camera.rotY((now.x - prev.x) * sensivity);
			camera.rotX(-(now.y - prev.y) * sensivity);
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

		if (handler.getKeyState(GLFW_KEY_RIGHT)) {
			c1.pos.x += 3.0f * delta;
		}
		else if (handler.getKeyState(GLFW_KEY_LEFT)) {
			c1.pos.x -= 3.0f * delta;
		}

		if (handler.getKeyState(GLFW_KEY_DOWN)) {
			c1.pos.z += 3.0f * delta;
		}
		else if (handler.getKeyState(GLFW_KEY_UP)) {
			c1.pos.z -= 3.0f * delta;
		}

		if (handler.getKeyState(GLFW_KEY_K)) {
			c1.pos.y += 3.0f * delta;
		}
		else if (handler.getKeyState(GLFW_KEY_M)) {
			c1.pos.y -= 3.0f * delta;
		}
	}
};