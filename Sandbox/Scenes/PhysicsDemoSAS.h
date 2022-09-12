#pragma once

#include <Edge3D.h>
#include<vector>
#include <chrono>
#include <thread>

#define AREA_COUNT 225

class PhysicsDemo : public Edge3D::Scene {
private:
	struct Ball {
		Vec3 pos;
		Vec3 velocity;
		float radius;
	};

	struct GLBall {
		Vec3 pos;
		float radius;
		Material materrial;
	};

	struct Rect {
		Vec3 min;
		Vec3 max;
	};

	struct Area {
		Rect rect;

		std::vector<unsigned int> items;
	};

	float width;
	float height = 100;

	Edge3D::Window& window;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::PerspectiveCamera camera;
	Edge3D::InputHandler handler;

	Edge3D::FrameBuffer* fb = nullptr;

	Vec2 now, prev;
	bool isClicked = false;
	float sensivity = 0.045f;
	float cameraSpeed = 10.0f;

	Edge3D::SphereMesh mesh;
	Edge3D::VertexArray va;
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::Shader* shader = nullptr;

	uint32_t uniform_buffer;

	DirectionalLight light;
	Material material;

	float radius = 0.5f;
	int vSegment = 20, hSegment = 20;

	std::vector<Ball> balls;
	std::vector<GLBall> glBalls;

	Edge3D::VertexBuffer* instanceVB = nullptr;

	float gravity;
	float waitingDuration = 0;
	int count = 100;

	//Energy Consevation
	float EC_Ground;
	float EC_Wall;
	float EC_Ball;

	MersenneTwister mt;

	Material planeMaterial;
	Vec3 planeColor = { 1,1,1 };
	Matrix4x4 pModel;
	Edge3D::VertexBuffer* pvb = nullptr;
	Edge3D::VertexArray pva;
	Edge3D::Shader* pShader = nullptr;

	bool isTPressed = false;
	float speed = 100;
	bool vsync = true;

	bool randomColor = true;
	Vec3 ballColor = { 1,0,0 };

	bool isAltPressed = false;
	bool stop = false;

	std::thread* updatethread1 = nullptr;
	float updateTime1 = 0;
	std::thread* updatethread2 = nullptr;
	float updateTime2 = 0;

public:
	PhysicsDemo(Edge3D::Window& window, std::string name) : window(window), Scene(name), skybox("assets/rooitou_park.jpg"), camera(70, 9.0f / 16.0f, 0.1f, 500) {

		mesh.calculate();
		vb = new Edge3D::VertexBuffer(nullptr, 100000, GL_STATIC_DRAW);
		ib = new Edge3D::IndexBuffer(nullptr, 100000, GL_STATIC_DRAW);

		vb->subData(0, mesh.getVerticesSize(), (float*)mesh.getVertices());
		ib->subData(0, mesh.getIndicesSize(), mesh.getIndices());

		va.bind();
		vb->bind();
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		shader = new Edge3D::Shader("Shaders/physicsdemo.shader");

		fb = new Edge3D::FrameBuffer(600, 600);

		glGenBuffers(1, &uniform_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix4x4), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);

		shader->bindUniformBuffer("Matrices", 0);

		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4), sizeof(Matrix4x4), camera.getProjectionMatrix());

		light.direction = { 0,-1,0 };
		light.ambient = { 0.3f,0.3f,0.3f };
		light.diffuse = { 0.5f,0.5f,0.5f };
		light.specular = { 0.7f,0.7f,0.7f };

		material.ambient = { 0.3f,0.0f,0.0f };
		material.diffuse = { 0.5f,0.0f,0.0f };
		material.specular = { 0.7f,0.7f,0.7f };
		material.shininess = 50;

		width = 16;

		gravity = -10.0f;
		EC_Ground = 0.7f;
		EC_Wall = 0.7f;
		EC_Ball = 0.99f;

		Vec3 planeVertices[] = {
			{0.5f,0,0.5f},{0,1,0},
			{0.5f,0,-0.5f},{0,1,0},
			{-0.5f,0,-0.5f},{0,1,0},

			{0.5f,0,0.5f},{0,1,0},
			{-0.5f,0,-0.5f},{0,1,0},
			{-0.5f,0,0.5f},{0,1,0}
		};

		planeMaterial.ambient = { 0.0f,0.0f,3.0f };
		planeMaterial.diffuse = { 0.0f,0.0f,5.0f };
		planeMaterial.specular = { 0.3f,0.3f,0.3f };
		planeMaterial.shininess = 20;

		pvb = new Edge3D::VertexBuffer((float*)planeVertices, 12 * sizeof(Vec3), GL_STATIC_DRAW);
		pva.addVertexAttribute(pvb, 3, GL_FLOAT, false);
		pva.addVertexAttribute(pvb, 3, GL_FLOAT, false);

		pShader = new Edge3D::Shader("Shaders/plane.shader");

		instanceVB = new Edge3D::VertexBuffer(nullptr, 100000 * sizeof(GLBall), GL_DYNAMIC_DRAW);

		camera.setPosition(0, 1, 0);

		float cursorVertices[8]{
			0.0f,0.1f,  0.0f,-0.1f,
			-0.1f, 0.0f, 0.1f,0.0f
		};

	}

	~PhysicsDemo() {
		delete vb;
		delete ib;
		delete shader;
		delete fb;

		delete pvb;
		delete pShader;

		delete instanceVB;
	}

	void onCreate() {
		APP_LOG_INFO("Scene onCreate function called, id: {0}", id);
	}

	void onUpdate(float delta) {
		input(delta);
		camera.update(delta);

		planeMaterial.ambient = planeColor * 0.3f;
		planeMaterial.diffuse = planeColor * 0.5f;

		glfwSwapInterval(vsync ? 1 : 0);

		if (vSegment != mesh.getVerticalSegment() || hSegment != mesh.getHorizontalSegment()) {
			mesh.setSegment(vSegment, hSegment);
			mesh.calculate();
			vb->subData(0, mesh.getVerticesSize(), (float*)mesh.getVertices());
			ib->subData(0, mesh.getIndicesSize(), mesh.getIndices());
		}

		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);
		glViewport(0, 0, fb->getWidth(), fb->getHeight());
		glEnable(GL_CULL_FACE);

		glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4), camera.getViewMatrix());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4), sizeof(Matrix4x4), camera.getProjectionMatrix());

		skybox.draw(camera);

		va.bind();
		ib->bind();
		shader->bind();

		shader->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
		shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
		shader->setUniform1f("light.intensity", light.intensity);
		shader->setUniform3f("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		for (unsigned int i = 0; i < balls.size(); i++) {
			glBalls[i].pos = balls[i].pos;
			glBalls[i].radius = balls[i].radius;
		}
		instanceVB->bind();
		instanceVB->subData(0, sizeof(GLBall) * glBalls.size(), (float*)glBalls.data());

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(GLBall), 0);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(GLBall), (void*)sizeof(Vec3));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(GLBall), (void*)(4 * sizeof(float)));

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, false, sizeof(GLBall), (void*)(7 * sizeof(float)));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, false, sizeof(GLBall), (void*)(10 * sizeof(float)));

		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 1, GL_FLOAT, false, sizeof(GLBall), (void*)(13 * sizeof(float)));

		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glDrawElementsInstanced(GL_TRIANGLES, mesh.getIndicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0, glBalls.size());

		pva.bind();
		pShader->bind();

		pShader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		pShader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());


		pShader->setUniform3f("material.ambient", planeMaterial.ambient.x, planeMaterial.ambient.y, planeMaterial.ambient.z);
		pShader->setUniform3f("material.diffuse", planeMaterial.diffuse.x, planeMaterial.diffuse.y, planeMaterial.diffuse.z);
		pShader->setUniform3f("material.specular", planeMaterial.specular.x, planeMaterial.specular.y, planeMaterial.specular.z);
		pShader->setUniform1f("material.shininess", planeMaterial.shininess);

		pShader->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
		pShader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		pShader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		pShader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
		pShader->setUniform1f("light.intensity", light.intensity);

		glDisable(GL_CULL_FACE);

		pModel = Math::scale(width * 2.0f, 1, width * 2.0f);
		pShader->setUniformMatrix4fv("model", 1, false, pModel.m[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glEnable(GL_CULL_FACE);

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

		if (updatethread1 != nullptr) {
			stop = true;
			updatethread1->join();
			delete updatethread1;
			updatethread1 = nullptr;

			updatethread2->join();
			delete updatethread2;
			updatethread2 = nullptr;

			stop = false;
		}
	}

	void check() {
		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL Error: error code = {0}", error);
		}
	}

	void input(float delta) {
		ImGui::Begin("Properties");

		ImGui::Checkbox("Vsync", &vsync);
		ImGui::Text("Main thread");
		ImGui::Text("FPS: %.2f  frame time: %.2fms", 1.0f / delta, delta * 1000.0f);

		ImGui::Separator();
		ImGui::Text("Physics Threads");
		ImGui::Text("Physics thread1: %.2fms",updateTime1 * 1000.0f);
		ImGui::Text("Physics thread2: %.2fms", updateTime2 * 1000.0f);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("number of balls: %d", balls.size());

		ImGui::Separator();
		ImGui::Text("Light Properties");
		ImGui::SliderFloat3("direction", (float*)&light.direction, -1, 1);
		ImGui::SliderFloat3("ambient", (float*)&light.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&light.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&light.specular, 0, 1);
		ImGui::SliderFloat("intensity", &light.intensity, 0, 3);

		ImGui::Separator();
		ImGui::Text("Ball Segmentation");
		ImGui::SliderInt("Horizontal", &vSegment, 3, 50);
		ImGui::SliderInt("Vertical", &hSegment, 2, 50);

		ImGui::Separator();
		ImGui::Text("Area Size");
		ImGui::SliderFloat("Width", &width, 2, 50);
		ImGui::SliderFloat("Height", &height, 10, 300);

		ImGui::Separator();
		ImGui::Text("Energy Conservation");
		ImGui::SliderFloat("Ground", &EC_Ground, 0, 2);
		ImGui::SliderFloat("Wall", &EC_Wall, 0, 2);
		ImGui::DragFloat("Balls", &EC_Ball, 0.001f, 0, 1.1f);

		ImGui::Separator();
		ImGui::Text("Number of ball will be added");
		ImGui::SliderInt("Count", &count, 2, 2000);
		ImGui::Checkbox("Random Color", &randomColor);
		if (!randomColor) {
			ImGui::ColorEdit3("Ball Color", (float*)&ballColor);
		}

		ImGui::Separator();
		ImGui::SliderFloat("Gravity", &gravity, -100, -0);
		ImGui::SliderFloat("Fire Speed", &speed, 2, 400);

		ImGui::Separator();
		ImGui::ColorEdit3("Plane Color", (float*)&planeColor);

		ImGui::Separator();
		ImGui::Text("Right Click: Look around");
		ImGui::Text("W,A,S,D: Move");
		ImGui::Text("Space,Shift: Ascend/Descend");
		ImGui::Text("F: Add balls");
		ImGui::Text("C: Delete balls");
		ImGui::Text("Q,E: Rotate balls");
		ImGui::Text("T: Fire a ball");
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

		waitingDuration += delta;

		if (handler.getKeyState(GLFW_KEY_F) && waitingDuration >= 0.5f) {

			if (updatethread1 != nullptr && updatethread2 != nullptr) {
				stop = true;
				updatethread1->join();
				delete updatethread1;
				updatethread1 = nullptr;

				updatethread2->join();
				delete updatethread2;
				updatethread2 = nullptr;

				stop = false;
			}

			Material material;
			material.specular = { 0.9f,0.9f,0.9f };

			for (unsigned int i = 0; i < count; i++) {
				Ball ball;
				ball.pos = {
					1.0f * mt.nextFloat() * width - width,
					width + mt.nextFloat() * 6.0f - 3.0f,
					1.0f * mt.nextFloat() * width - width
				};

				if (randomColor) {
					material.diffuse = {
					mt.nextFloat(),
					mt.nextFloat(),
					mt.nextFloat()
					};

					material.ambient = material.diffuse;
					material.ambient.x /= 1.3f;
					material.ambient.y /= 1.3f;
					material.ambient.z /= 1.3f;

					material.shininess = 120;
				}
				else {
					material.diffuse = { ballColor.x, ballColor.y, ballColor.z };
					material.ambient = material.diffuse;
					material.ambient.x /= 1.3f;
					material.ambient.y /= 1.3f;
					material.ambient.z /= 1.3f;

					material.shininess = 120;
				}

				ball.radius = radius;
				ball.velocity = {
					mt.nextFloat() * 10.0f - 5.0f,
					mt.nextFloat() * 10.0f - 5.0f,
					mt.nextFloat() * 10.0f - 5.0f
				};

				balls.push_back(ball);
				glBalls.push_back({});
				glBalls[glBalls.size() - 1].materrial = material;
			}
			waitingDuration = 0;

			updatethread1 = new std::thread(updateBalls1,&balls,&width,&height,&EC_Ground,&EC_Wall,&gravity,&stop,&updateTime1);
			updatethread2 = new std::thread(updateBalls2, &balls, &width, &height, &EC_Ball, &stop, &updateTime2);
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			if (updatethread1 != nullptr) {
				stop = true;
				updatethread1->join();
				delete updatethread1;
				updatethread1 = nullptr;

				updatethread2->join();
				delete updatethread2;
				updatethread2 = nullptr;

				stop = false;
			}

			balls.clear();
			glBalls.clear();
		}

		if (handler.getKeyState(GLFW_KEY_T) && !isTPressed) {
			if (updatethread1 != nullptr) {
				stop = true;
				updatethread1->join();
				delete updatethread1;
				updatethread1 = nullptr;

				updatethread2->join();
				delete updatethread2;
				updatethread2 = nullptr;

				stop = false;
			}

			isTPressed = true;
			Ball ball;
			ball.pos = camera.getPosition() + camera.getDirection() * 2;

			if (randomColor) {
				material.diffuse = {
				mt.nextFloat(),
				mt.nextFloat(),
				mt.nextFloat()
				};

				material.ambient = material.diffuse;
				material.ambient.x /= 1.3f;
				material.ambient.y /= 1.3f;
				material.ambient.z /= 1.3f;

				material.shininess = 120;
			}
			else {
				material.diffuse = { ballColor.x, ballColor.y, ballColor.z };
				material.ambient = material.diffuse;
				material.ambient.x /= 1.3f;
				material.ambient.y /= 1.3f;
				material.ambient.z /= 1.3f;

				material.shininess = 120;
			}


			ball.radius = radius;
			ball.velocity = camera.getDirection() * speed;

			balls.push_back(ball);
			glBalls.push_back({});
			glBalls[glBalls.size() - 1].materrial = material;

			updatethread1 = new std::thread(updateBalls1, &balls, &width, &height, &EC_Ground, &EC_Wall, &gravity, &stop, &updateTime1);
			updatethread2 = new std::thread(updateBalls2, &balls, &width, &height, &EC_Ball, &stop, &updateTime2);
		}
		else if (!handler.getKeyState(GLFW_KEY_T)) {
			isTPressed = false;
		}

		if (handler.getKeyState(GLFW_KEY_E)) {

			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({ -ball.pos.x,0,-ball.pos.z });
				float length = Vec3({ -ball.pos.x,0,-ball.pos.z }).length();

				ball.velocity.x += direction.z * 0.9f * delta * 60.0f;
				ball.velocity.z -= direction.x * 0.9f * delta * 60.0f;
				ball.velocity.y += 0.07f / length;
			}
		}

		if (handler.getKeyState(GLFW_KEY_Q)) {
			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({ -ball.pos.x,0,-ball.pos.z });
				float slope = atan2f(ball.pos.z, ball.pos.x);

				ball.velocity.x -= direction.z * 0.1f * delta * 60.0f - 0.2f * PI * direction.x * delta * 60.0f;
				ball.velocity.z += direction.x * 0.1f * delta * 60.0f + 0.2f * PI * direction.z * delta * 60.0f;
				ball.velocity.y += 0.1f * delta * 60.0f;
			}
		}

		if (handler.getKeyState(GLFW_KEY_LEFT_ALT) && !isAltPressed) {
			isAltPressed = true;
			if (stop == false) {
				stop = true;
				updatethread1->join();
				delete updatethread1;
				updatethread1 = nullptr;

				updatethread2->join();
				delete updatethread2;
				updatethread2 = nullptr;
			}
			else {
				stop = false;
				updatethread1 = new std::thread(updateBalls1, &balls, &width, &height, &EC_Ground, &EC_Wall, &gravity, &stop, &updateTime1);
				updatethread2 = new std::thread(updateBalls2, &balls, &width, &height, &EC_Ball, &stop, &updateTime2);
			}
			
		}
		else if (!handler.getKeyState(GLFW_KEY_LEFT_ALT) && isAltPressed) {
			isAltPressed = false;
		}
	}


	static void updateBalls2(std::vector<Ball>* balls, float* width, float* height,
		float* EC_Ball, bool* stop, float* updatetime2) {
		double delta = 0;

		std::vector<Area> areas;

		while (!(*stop)) {

			auto clock1 = std::chrono::high_resolution_clock::now();

			if ((*balls).size() <= 1000)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));

			//unsigned int size = (uint32_t)sqrtf(areas->size() / 1.0f);
			//
			//for (unsigned int j = 0; j < 1; j++) {
			//	for (unsigned int i = 0; i < size * size; i++) {
			//		Area& area = (*areas)[j * size * size + i];
			//		area.rect.min = { -*width + (unsigned int)(i / size) * (2.0f * *width) / size,j * (*height / 1.0f), -*width + (i % //size) * (2.0f * *width) / size };
			//		area.rect.max = { area.rect.min.x + (2.0f * *width) / size ,area.rect.min.y + *height / 1.0f,
			//			area.rect.min.z + (2.0f * *width) / size };
			//	}
			//}

			float s = (*width * 2.0f) / 4.0f;
			int size = (int)s + 1;
			if (s - (int)s == 0.0f)
				size = (int)s;

			//APP_LOG_WARN("Size: {0}", size);

			for (Area& area : areas) {
				area.items.clear();
				area.items.shrink_to_fit();
			}
			areas.clear();
			areas.shrink_to_fit();

			for (uint32_t i = 0; i < size; i++) {
				for (uint32_t j = 0; j < size; j++) {
					Area area;
					area.rect.min = { -*width + (float)i * 4.0f, 0, -*width + (float)j * 4.0f };
					area.rect.max = { -*width + (float)(i + 1) * 4.0f, *height, -*width + (float)(j + 1) * 4.0f };
					areas.push_back(area);
				}
			}

			for (uint32_t i = 0; i < (*balls).size(); i++) {
				Rect rect;
				rect.min = { (*balls)[i].pos - Vec3{(*balls)[i].radius,(*balls)[i].radius,(*balls)[i].radius} };
				rect.max = { (*balls)[i].pos + Vec3{(*balls)[i].radius,(*balls)[i].radius,(*balls)[i].radius} };

				int i1, j1;
				i1 = (int)((*width + rect.min.x) / 4.0f);
				j1 = (int)((*width + rect.min.z) / 4.0f);

				int i2, j2;
				i2 = (int)((*width + rect.max.x) / 4.0f);
				j2 = (int)((*width + rect.max.z) / 4.0f);
				
				if(i1 >= 0 && j1 >= 0 && i1 < size && j1 < size)
					areas[i1 * size + j1].items.push_back(i);
				
				if(i1 != i2 && j1 == j2 && i2 >= 0 && j1 >= 0 && i2 < size && j1 < size)
					areas[i2 * size + j1].items.push_back(i);

				if (i1 == i2 && j1 != j2 && i1 >= 0 && i1 < size && j2 >= 0 && j2 < size)
					areas[i1 * size + j2].items.push_back(i);

				if (i1 != i2 && j1 != j2 && i2 >= 0 && j2 >= 0 && i2 < size && j2 < size)
					areas[i2 * size + j2].items.push_back(i);
			}

			for (uint32_t k = 0; k < areas.size(); k++) {

				for (uint32_t i = 0; i < areas[k].items.size(); i++) {
					Ball& ball = (*balls)[areas[k].items[i]];

					for (uint32_t j = i + 1; j < areas[k].items.size(); j++) {

						Ball& ball2 = (*balls)[areas[k].items[j]];

						float lenSq = (ball.pos - ball2.pos).lengthSquare();

						if (lenSq < (ball.radius + ball2.radius) * (ball.radius + ball2.radius)) {

							float length = (ball.pos - ball2.pos).length();
							float diff = ball.radius + ball2.radius - length;

							Vec3 direction = Math::normalize(ball2.pos - ball.pos);

							ball.pos = ball.pos - direction * diff * 0.51f;
							ball2.pos = ball2.pos + direction * diff * 0.51f;

							Ball temp = ball;

							ball.velocity = (ball.velocity - (Math::dotProduct(ball.velocity - ball2.velocity, ball.pos - ball2.pos) / (ball.pos - ball2.pos).lengthSquare()) * (ball.pos - ball2.pos)) * *EC_Ball;
							ball2.velocity = (ball2.velocity - (Math::dotProduct(ball2.velocity - temp.velocity, ball2.pos - temp.pos) / (ball2.pos - temp.pos).lengthSquare()) * (ball2.pos - temp.pos)) * *EC_Ball;
						}
					}
				}
			}

			auto clock2 = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::microseconds> (clock2 - clock1).count();

			delta = duration / 1000000.0;
			*updatetime2 = (float)delta;
		}
	}

	static inline bool collide(const Rect& a, const Rect& b) {
		return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
	}

	static inline bool fully_collide(const Rect& a, const Rect& b) {
		return (b.min.x >= a.min.x && a.max.x >= b.max.x) &&
			(b.min.y >= a.min.y && a.max.y >= b.max.y) &&
			(b.min.z >= a.min.z && a.max.z >= b.max.z);
	}

	static void updateBalls1(std::vector<Ball>* balls, float* width, float* height,
		float* EC_Ground, float* EC_Wall, float* gravity, bool* stop, float* updatetime1) {
		float delta = 0;

		while (!(*stop)) {
			auto clock1 = std::chrono::high_resolution_clock::now();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			for (unsigned int i = 0; i < balls->size(); i++) {
				Ball& ball = (*balls)[i];

				if (ball.pos.y != ball.radius && ball.velocity.y != 0.0f) {
					ball.velocity.y += *gravity * delta;
				}

				ball.pos.x += ball.velocity.x * delta;
				ball.pos.y += ball.velocity.y * delta;
				ball.pos.z += ball.velocity.z * delta;

				if (ball.pos.y - ball.radius <= 0) {
					if (ball.velocity.y < 0.1f) {
						ball.pos.y = ball.radius - (ball.pos.y - ball.radius);
						ball.velocity.y *= -*EC_Ground;
					}
					else {
						ball.pos.y = ball.radius;
						ball.velocity.y = 0;
					}

				}
				else if (ball.pos.y + ball.radius >= *height) {
					ball.pos.y = (*height - ball.radius) - (ball.pos.y + ball.radius - *height);
					ball.velocity.y *= -*EC_Ground;
				}

				if (ball.pos.x - ball.radius <= -*width) {
					if (ball.velocity.x < -0.1f) {
						ball.velocity.x *= -*EC_Wall;
						ball.pos.x = -2.0f * *width - ball.pos.x + 2.0f * ball.radius;
					}
					else {
						ball.velocity.x = 0;
						ball.pos.x = -*width + ball.radius;
					}
				}
				else if (ball.pos.x + ball.radius >= *width) {
					if (ball.velocity.x > 0.1f) {
						ball.velocity.x *= -*EC_Wall;
						ball.pos.x = 2.0f * *width - ball.pos.x - 2.0f * ball.radius;
					}
					else {
						ball.velocity.x = 0;
						ball.pos.x = *width - ball.radius;
					}
				}


				if (ball.pos.z - ball.radius <= -*width) {
					if (ball.velocity.z < -0.1f) {
						ball.velocity.z *= -*EC_Wall;
						ball.pos.z = -2.0f * *width - ball.pos.z + 2.0f * ball.radius;
					}
					else {
						ball.velocity.z = 0;
						ball.pos.z = -*width + ball.radius;
					}
				}
				else if (ball.pos.z + ball.radius >= *width) {
					if (ball.velocity.z > 0.1f) {
						ball.velocity.z *= -*EC_Wall;
						ball.pos.z = 2.0f * *width - ball.pos.z - 2.0f * ball.radius;
					}
					else {
						ball.velocity.z = 0;
						ball.pos.z = *width - ball.radius;
					}
				}
			}

			auto clock2 = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::microseconds> (clock2 - clock1).count();

			delta = duration / 1000000.0f;
			*updatetime1 = delta;
		}
	}
};



/*
double delta = 0;

		while (!(*stop)) {

			auto clock1 = std::chrono::high_resolution_clock::now();

			if ((*balls).size() <= 1000)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));

			//unsigned int size = (uint32_t)sqrtf(areas->size() / 1.0f);
			//
			//for (unsigned int j = 0; j < 1; j++) {
			//	for (unsigned int i = 0; i < size * size; i++) {
			//		Area& area = (*areas)[j * size * size + i];
			//		area.rect.min = { -*width + (unsigned int)(i / size) * (2.0f * *width) / size,j * (*height / 1.0f), -*width + (i % //size) * (2.0f * *width) / size };
			//		area.rect.max = { area.rect.min.x + (2.0f * *width) / size ,area.rect.min.y + *height / 1.0f,
			//			area.rect.min.z + (2.0f * *width) / size };
			//	}
			//}

			float s = *width / 4.0f;
			int size = (int)s + 1;
			if (s - (int)s == 0.0f)
				size = (int)s;


			for (auto& i : *areas)
				i.items.clear();

			for (uint32_t i = 0; i < (*balls).size(); i++) {
				Rect rect;
				rect.min = { (*balls)[i].pos - Vec3{(*balls)[i].radius,(*balls)[i].radius,(*balls)[i].radius} };
				rect.max = { (*balls)[i].pos + Vec3{(*balls)[i].radius,(*balls)[i].radius,(*balls)[i].radius} };

				for (uint32_t j = 0; j < (*areas).size(); j++) {
					if (fully_collide((*areas)[j].rect, rect)) {
						(*areas)[j].items.push_back(i);
						break;
					}

					if (collide(rect, (*areas)[j].rect)) {
						(*areas)[j].items.push_back(i);
					}
				}
			}



			for (uint32_t k = 0; k < areas->size(); k++) {

				for (uint32_t i = 0; i < (*areas)[k].items.size(); i++) {
					Ball& ball = (*balls)[(*areas)[k].items[i]];

					for (uint32_t j = i + 1; j < (*areas)[k].items.size(); j++) {

						Ball& ball2 = (*balls)[(*areas)[k].items[j]];

						float lenSq = (ball.pos - ball2.pos).lengthSquare();

						if (lenSq < (ball.radius + ball2.radius) * (ball.radius + ball2.radius)) {

							float length = (ball.pos - ball2.pos).length();
							float diff = ball.radius + ball2.radius - length;

							Vec3 direction = Math::normalize(ball2.pos - ball.pos);

							ball.pos = ball.pos - direction * diff * 0.51f;
							ball2.pos = ball2.pos + direction * diff * 0.51f;

							Ball temp = ball;

							ball.velocity = (ball.velocity - (Math::dotProduct(ball.velocity - ball2.velocity, ball.pos - ball2.pos) / (ball.pos - ball2.pos).lengthSquare()) * (ball.pos - ball2.pos)) * *EC_Ball;

							ball2.velocity = (ball2.velocity - (Math::dotProduct(ball2.velocity - temp.velocity, ball2.pos - temp.pos) / (ball2.pos - temp.pos).lengthSquare()) * (ball2.pos - temp.pos)) * *EC_Ball;
						}
					}
				}
			}

			auto clock2 = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::microseconds> (clock2 - clock1).count();

			delta = duration / 1000000.0;
			*updatetime2 = (float)delta;
		}
*/