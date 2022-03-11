#pragma once

#include <Edge3D.h>
#include<vector>
#include <random>
#include <chrono>
#include <thread>

static float frametime;

class MeshTest : public Edge3D::Scene {
private:
	struct Ball {
		Vec3 pos;
		Vec3 velocity;
		Material material;
		float radius;
	};

	struct GLBall {
		Vec3 pos;
		float radius;
		Material materrial;
	};

	float width;
	float height = 100;

	Edge3D::Window& window;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::PerspectiveCamera camera;
	Edge3D::InputHandler handler;
	Vec2 now, prev;
	bool isClicked = false;
	float sensivity = 0.025f;
	float cameraSpeed = 10.0f;

	Edge3D::SphereMesh mesh;
	Edge3D::VertexArray va;
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::Shader* shader = nullptr;
	Edge3D::FrameBuffer* fb = nullptr;

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

	float conservationV;
	float conservationH;
	float conservationB;

	std::default_random_engine engine;
	std::uniform_real_distribution<float> distribution;

	bool stop = false;
	std::thread* t1 = nullptr;

	Material planeMaterial;
	Vec3 planeColor = { 1,1,1 };
	Edge3D::VertexBuffer* pvb = nullptr;
	Edge3D::VertexArray pva;
	Edge3D::Shader* pShader = nullptr;

	bool isTPressed = false;
	float speed = 100;
	bool vsync = true;
public:
	MeshTest(Edge3D::Window& window, std::string name) : window(window), Scene(name), skybox("assets/whale_skeleton.jpg"), camera(70, 9.0f / 16.0f, 0.1f, 500) {

		mesh.calculate();
		vb = new Edge3D::VertexBuffer(nullptr, 100000, GL_STATIC_DRAW);
		ib = new Edge3D::IndexBuffer(nullptr, 100000, GL_STATIC_DRAW);

		vb->subData(0, mesh.getVerticesSize(), (float*)mesh.getVertices());
		ib->subData(0, mesh.getIndicesSize(), mesh.getIndices());

		va.bind();
		vb->bind();
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		shader = new Edge3D::Shader("Shaders/MeshTest.shader");

		fb = new Edge3D::FrameBuffer(600,600);

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
		conservationV = 0.7f; conservationH = 0.7f;
		conservationB = 0.99f;

		t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);

		Vec3 planeVertices[] = {
			{0.5f,0,0.5f},{0,1,0},
			{0.5f,0,-0.5f},{0,1,0},
			{-0.5f,0,-0.5f},{0,1,0},

			{0.5f,0,0.5f},{0,1,0},
			{-0.5f,0,0.5f},{0,1,0},
			{-0.5f,0,-0.5f},{0,1,0}
		};

		planeMaterial.ambient = { 0.0f,0.0f,3.0f };
		planeMaterial.diffuse = { 0.0f,0.0f,5.0f };
		planeMaterial.specular = { 0.1f,0.1f,0.1f };
		planeMaterial.shininess = 20;

		pvb = new Edge3D::VertexBuffer((float*)planeVertices, 12 * sizeof(Vec3), GL_STATIC_DRAW);
		pva.addVertexAttribute(pvb, 3, GL_FLOAT, false);
		pva.addVertexAttribute(pvb, 3, GL_FLOAT, false);

		pShader = new Edge3D::Shader("Shaders/plane.shader");

		instanceVB = new Edge3D::VertexBuffer(nullptr, 10000 * sizeof(GLBall), GL_DYNAMIC_DRAW);
	}

	~MeshTest() {
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
			glBalls[i].materrial = balls[i].material;
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

		pShader->setUniformMatrix4fv("view",1,true,camera.getViewMatrix());
		pShader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		pShader->setUniform3f("scale", width * 2.0f, 0, width * 2.0f);
		pShader->setUniform3f("material.ambient", planeMaterial.ambient.x, planeMaterial.ambient.y, planeMaterial.ambient.z);
		pShader->setUniform3f("material.diffuse", planeMaterial.diffuse.x, planeMaterial.diffuse.y, planeMaterial.diffuse.z);
		pShader->setUniform3f("material.specular", planeMaterial.specular.x, planeMaterial.specular.y, planeMaterial.specular.z);
		pShader->setUniform1f("material.shininess", planeMaterial.shininess);

		pShader->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
		pShader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		pShader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		pShader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
		pShader->setUniform1f("light.intensity", light.intensity);

		glDrawArrays(GL_TRIANGLES, 0, 6);

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

	static void updateBalls(std::vector<Ball>* balls, float* width, float* height, float* conservationV, float* conservationH, float* conservationB, float* gravity, bool* stop) {
		float delta = 0;
		float prev, now;
		auto begin = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

		while (!(*stop)) {
			begin = std::chrono::high_resolution_clock::now();

			for (unsigned int i = 0; i < balls->size(); i++) {
				Ball& ball = (*balls)[i];

				if (ball.pos.y - ball.radius <= 0) {
					if (ball.velocity.y < 0.1f) {
						ball.pos.y = ball.radius - (ball.pos.y - ball.radius);
						ball.velocity.y *= -(*conservationV);
					}
					else {
						ball.pos.y = ball.radius;
						ball.velocity.y = 0;
					}

				}
				else if (ball.pos.y + ball.radius >= *height) {
					ball.pos.y = (*height - ball.radius) - (ball.pos.y + ball.radius - *height);
					ball.velocity.y *= -(*conservationV);
				}

				if (ball.pos.x - ball.radius <= -(*width)) {
					if (ball.velocity.x < -0.1f) {
						ball.velocity.x *= -(*conservationH);
						ball.pos.x = -2.0f * (*width) - ball.pos.x + 2.0f * ball.radius;
					}
					else {
						ball.velocity.x = 0;
						ball.pos.x = -(*width) + ball.radius;
					}
				}
				else if (ball.pos.x + ball.radius >= (*width)) {
					if (ball.velocity.x > 0.1f) {
						ball.velocity.x *= -(*conservationH);
						ball.pos.x = 2.0f * (*width) - ball.pos.x - 2.0f * ball.radius;
					}
					else {
						ball.velocity.x = 0;
						ball.pos.x = (*width) - ball.radius;
					}
				}


				if (ball.pos.z - ball.radius <= -(*width)) {
					if (ball.velocity.z < -0.1f) {
						ball.velocity.z *= -(*conservationH);
						ball.pos.z = -2.0f * (*width) - ball.pos.z + 2.0f * ball.radius;
					}
					else {
						ball.velocity.z = 0;
						ball.pos.z = -(*width) + ball.radius;
					}
				}
				else if (ball.pos.z + ball.radius >= (*width)) {
					if (ball.velocity.z > 0.1f) {
						ball.velocity.z *= -(*conservationH);
						ball.pos.z = 2.0f * (*width) - ball.pos.z - 2.0f * ball.radius;
					}
					else {
						ball.velocity.z = 0;
						ball.pos.z = (*width) - ball.radius;
					}
				}


				for (unsigned int j = i + 1; j < balls->size(); j++) {
					Ball& ball2 = (*balls)[j];

					float rad = ball.radius + ball2.radius;

					float d = Vec3(ball.pos - ball2.pos).lengthSquare();
					if (d <= rad * rad) {
						d = sqrtf(d);
						float overlap = 0.55f * (d - ball.radius - ball2.radius);
						Ball temp = ball;                        
						ball.pos.x -= overlap * (ball.pos.x - ball2.pos.x) / d;
						ball.pos.y -= overlap * (ball.pos.y - ball2.pos.y) / d;
						ball.pos.z -= overlap * (ball.pos.z - ball2.pos.z) / d;

						ball2.pos.x += overlap * (temp.pos.x - ball2.pos.x) / d;
						ball2.pos.y += overlap * (temp.pos.y - ball2.pos.y) / d;
						ball2.pos.z += overlap * (temp.pos.z - ball2.pos.z) / d;

						float disX = ball2.pos.x - ball.pos.x;
						float disY = ball2.pos.y - ball.pos.y;
						float disZ = ball2.pos.z - ball.pos.z;

						d = Vec3(ball.pos - ball2.pos).length();

						float nx = disX / d;
						float ny = disY / d;
						float nz = disZ / d;

						float kx = ball.velocity.x - ball2.velocity.x;
						float ky = ball.velocity.y - ball2.velocity.y;
						float kz = ball.velocity.z - ball2.velocity.z;

						float p = 2.0f * (nx * kx + ny * ky + nz * kz) / 2.0f;

						ball.velocity.x = (ball.velocity.x - p * nx) * (*conservationB);
						ball.velocity.y = (ball.velocity.y - p * ny) * (*conservationB);
						ball.velocity.z = (ball.velocity.z - p * nz) * (*conservationB);

						ball2.velocity.x = (ball2.velocity.x + p * nx) * (*conservationB);
						ball2.velocity.y = (ball2.velocity.y + p * ny) * (*conservationB);
						ball2.velocity.z = (ball2.velocity.z + p * nz) * (*conservationB);

					}
				}

				if (ball.pos.y != ball.radius && ball.velocity.y != 0.0f) {
					ball.velocity.y += *gravity * (delta);
				}

				ball.pos.x += ball.velocity.x * (delta);
				ball.pos.y += ball.velocity.y * (delta);
				ball.pos.z += ball.velocity.z * (delta);
			}

			end = std::chrono::high_resolution_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

			delta = elapsed.count() / 1000000000.0f;
			frametime = delta;
		}
	}


	void onSuspend() {
		APP_LOG_INFO("Scene onSuspend function called, id: {0}", id);

		stop = true;
		t1->join();
	}

	void onResume() {
		APP_LOG_INFO("Scene onResume function called, id: {0}", id);
		t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
	}

	void onDestroy() {
		APP_LOG_INFO("Scene onDestroy function called, id: {0}", id);
		stop = true;
		t1->join();
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
		ImGui::NewLine();
		ImGui::Text("Physics thread");
		ImGui::Text("number of balls: %d", balls.size());
		ImGui::Text("update time: %.2fms", frametime * 1000.0f);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Light Properties");
		ImGui::SliderFloat3("direction", (float*)&light.direction, -1, 1);
		ImGui::SliderFloat3("ambient", (float*)&light.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&light.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&light.specular, 0, 1);
		ImGui::SliderFloat("intensity", &light.intensity, 0, 3);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Ball Segmentation");
		ImGui::SliderInt("Horizontal", &vSegment, 3, 50);
		ImGui::SliderInt("Vertical", &hSegment, 2, 50);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Area Size");
		ImGui::SliderFloat("Width", &width, 2, 50);
		ImGui::SliderFloat("Height", &height, 10, 300);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Energy Conservation");
		ImGui::SliderFloat("Ground", &conservationV, 0, 2);
		ImGui::SliderFloat("Wall", &conservationH, 0, 2);
		ImGui::SliderFloat("Balls", &conservationB, 0, 2);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::Text("Number of ball will be added");
		ImGui::SliderInt("Count", &count, 2, 500);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::SliderFloat("Gravity", &gravity, -100, -0);
		ImGui::SliderFloat("Fire Speed", &speed, 2, 400);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::ColorEdit3("Plane Color", (float*)&planeColor);

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
			stop = true;
			t1->join();
			delete t1;
			stop = false;

			Material material;
			material.specular = { 0.9f,0.9f,0.9f };

			for (unsigned int i = 0; i < count; i++) {
				Ball ball;
				ball.pos = {
					2.0f * distribution(engine) * width - width,
					width + distribution(engine) * 6.0f - 3.0f,
					2.0f * distribution(engine) * width - width
				};

				material.diffuse = {
					distribution(engine),
					distribution(engine),
					distribution(engine)
				};

				material.ambient = material.diffuse;
				material.ambient.x /= 1.3f;
				material.ambient.y /= 1.3f;
				material.ambient.z /= 1.3f;

				material.shininess = 20 + distribution(engine) * 180.0f;

				ball.material = material;
				ball.radius = radius;
				ball.velocity = {
					distribution(engine) * 10.0f - 5.0f,
					distribution(engine) * 10.0f - 5.0f,
					distribution(engine) * 10.0f - 5.0f
				};

				balls.push_back(ball);
				glBalls.push_back({});
			}
			waitingDuration = 0;

			t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			stop = true;
			t1->join();
			delete t1;
			stop = false;
			balls.clear();
			glBalls.clear();
			t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}

		if (handler.getKeyState(GLFW_KEY_T) && !isTPressed) {
			isTPressed = true;

			stop = true;
			t1->join();
			delete t1;
			stop = false;
			Ball ball;
			ball.pos = camera.getPosition() + camera.getDirection() * 2;

			material.diffuse = {
				distribution(engine),
				distribution(engine),
				distribution(engine)
			};

			material.ambient = material.diffuse;
			material.ambient.x /= 1.3f;
			material.ambient.y /= 1.3f;
			material.ambient.z /= 1.3f;

			material.shininess = 20 + distribution(engine) * 180.0f;

			ball.material = material;
			ball.radius = radius;
			ball.velocity = camera.getDirection() * speed;

			balls.push_back(ball);
			glBalls.push_back({});
			t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}
		else if (!handler.getKeyState(GLFW_KEY_T)){
			isTPressed = false;
		}

		if (handler.getKeyState(GLFW_KEY_E)) {
			//stop = true;
			//t1->join();
			//delete t1;
			//stop = false;

			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({ -ball.pos.x,0,-ball.pos.z });
				float length = Vec3({ -ball.pos.x,0,-ball.pos.z }).length();

				ball.velocity.x += direction.z * 0.9f * delta * 60.0f;
				ball.velocity.z -= direction.x * 0.9f * delta * 60.0f;
				ball.velocity.y += 0.07f / length;
			}

			//t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}

		if (handler.getKeyState(GLFW_KEY_Q)) {
			//stop = true;
			//t1->join();
			//delete t1;
			//stop = false;

			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({ -ball.pos.x,0,-ball.pos.z });
				float slope = atan2f(ball.pos.z, ball.pos.x);

				ball.velocity.x -= direction.z * 0.1f * delta * 60.0f - 0.2f * PI * direction.x * delta * 60.0f;
				ball.velocity.z += direction.x * 0.1f * delta * 60.0f + 0.2f * PI * direction.z * delta * 60.0f;
				ball.velocity.y += 0.1f * delta * 60.0f;
			}

			//t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}
	}
};
