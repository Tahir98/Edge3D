#pragma once
#include "Edge3D.h"
#include <vector>
#include <random>
#include <chrono>

#include <thread>

class DemoScene2 : public Edge3D::Scene {
private:
	struct Ball {
		Vec3 pos;
		Vec3 velocity;
		Material material;
		float radius;
	};

	GLFWwindow* window;

	float radius;

	Light light, light2;
	Material material;
	Edge3D::Sphere sphere;
	Edge3D::Sphere lightDirector;
	Edge3D::Cylinder cylinder;
	Edge3D::PerspectiveCamera camera;
	Edge3D::SpherecalSkybox skybox;

	Edge3D::InputHandler handler;
	bool isClicked = false;
	Vec2 prev;
	Vec2 now;
	float sensivity = 0.025f;
	float cameraSpeed = 10.0f;
	int count = 100;

	std::vector<Ball> balls;

	float gravity;
	float waitingDuration = 0;

	float conservationV;
	float conservationH;
	float conservationB;
	float fps;

	std::default_random_engine engine;
	std::uniform_real_distribution<float> distribution;

	bool isAdded = false;
	float delta = 0;
	std::thread* t1 = nullptr;
public:
	DemoScene2(GLFWwindow* window) : Edge3D::Scene("DemoScene"), window(window), camera(70, Edge3D::Application::aspectRatio, 0.1f, 300), skybox("assets/forest_slope.jpg"), distribution(0, 1) {

		radius = 16;

		gravity = -10.0f;
		conservationV = 0.7f; conservationH = 0.7f;
		conservationB = 0.99f;

		delta = 1.0f / 60.0f;

		t1 = new std::thread(updateBalls, &balls, &radius, &conservationV, &conservationH, &conservationB, &gravity, &isAdded);
	}

	void onCreate() {
		light.ambient = { 0.3f,0.3f,0.3f };
		light.diffuse = { 0.6f,0.6f,0.6f };
		light.specular = { 0.9f,0.9f,0.9f };

		material.ambient = { 0.3f,0.3f,0.3f };
		material.diffuse = { 0.9f,0.9f,0.9f };
		material.specular = { 0.0f,0.0f,0.0f };
		material.shininess = 100;

		cylinder.setMaterial(material);
		cylinder.setSize(radius,2*radius);
		cylinder.setSegment(50);

		lightDirector.debugMode = true;
		lightDirector.setPosition({ 0,10,0 });

		sphere.setSegment(10);
	}

	void onUpdate(float delta) {
		this->delta = delta;
		input(delta);
		camera.update(delta);


		if (sphere.getSegment() != lightDirector.getSegment()) {
			sphere.setSegment(lightDirector.getSegment());
		}

		light2.pos = light.pos = lightDirector.getPosition();

		skybox.draw(camera);

		lightDirector.draw(camera);

		cylinder.setPosition({ 0,radius,0 });
		cylinder.setSize(radius, 2 * radius);
		cylinder.draw(camera,light);

		for (Ball& ball : balls) {
			sphere.setPosition(ball.pos);
			sphere.setMaterial(ball.material);
			sphere.draw(camera, light);
		}

	}

	static void updateBalls(std::vector<Ball>* balls, float* radius, float* conservationV, float* conservationH, float* conservationB, float* gravity, bool* isAdded) {
		float delta = 0;
		float prev, now;
		auto begin = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

		while (!(*isAdded)) {
			begin = std::chrono::high_resolution_clock::now();

			for (unsigned int i = 0; i < balls->size(); i++) {
				Ball& ball = (*balls)[i];
				if (ball.pos.y != ball.radius && ball.velocity.y != 0.0f) {
					ball.velocity.y += *gravity * (delta);
				}

				ball.pos.x += ball.velocity.x * (delta);
				ball.pos.y += ball.velocity.y * (delta);
				ball.pos.z += ball.velocity.z * (delta);

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
				else if (ball.pos.y + ball.radius >= 200) {
					ball.pos.y = (200.0f - ball.radius) - (ball.pos.y + ball.radius - 200.0f);
					ball.velocity.y *= -(*conservationV);
				}

				if (Vec3({ ball.pos.z,0,ball.pos.z }).length() >= *radius) {
					ball.pos.x += 

					ball.velocity.x = -1.0f;
					ball.velocity.z = -1.0f;
				}



				for (unsigned int j = i + 1; j < balls->size(); j++) {
					Ball& ball2 = (*balls)[j];

					if (&ball2.pos.x != &ball.pos.x) {
						float d = Vec3(ball.pos - ball2.pos).length();
						if (d <= ball.radius + ball2.radius) {
							float overlap = 0.5f * (d - ball.radius - ball2.radius);
							ball.pos.x -= overlap * (ball.pos.x - ball2.pos.x) / d;
							ball.pos.y -= overlap * (ball.pos.y - ball2.pos.y) / d;
							ball.pos.z -= overlap * (ball.pos.z - ball2.pos.z) / d;

							ball2.pos.x += overlap * (ball.pos.x - ball2.pos.x) / d;
							ball2.pos.y += overlap * (ball.pos.y - ball2.pos.y) / d;
							ball2.pos.z += overlap * (ball.pos.z - ball2.pos.z) / d;

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
				}
			}

			//std::cout << "Delta: " << delta << std::endl;
			end = std::chrono::high_resolution_clock::now();
			elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

			delta = elapsed.count() / 1000000.0f;

		}
	}

	void input(float delta) {
		ImGui::Begin("Profiler");
		ImGui::Text("FPS = %.2f  Frame Time = %.4f", 1.0f / delta, delta);
		ImGui::Text("Number of balls : %d", balls.size());

		ImGui::Text("\nWalls and ground size");
		ImGui::SliderFloat("W", &radius, 2, 50);

		ImGui::Text("\nGround Enegy Conservation");
		ImGui::SliderFloat("V", &conservationV, 0, 2);

		ImGui::Text("\nWalls Enegy Conservation");
		ImGui::SliderFloat("H", &conservationH, 0, 2);

		ImGui::Text("\nCollision Enegy Conservation");
		ImGui::SliderFloat("B", &conservationB, 0, 2);

		ImGui::Text("\nCount");
		ImGui::SliderInt("Count", &count, 2, 500);

		ImGui::End();

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !isClicked) {
			prev = now = handler.getMousePos();
			isClicked = true;

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
			prev = now;
			now = handler.getMousePos();
		}
		else {
			isClicked = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_3) && waitingDuration >= 0.5f) {
			isAdded = true;
			t1->join();
			delete t1;
			isAdded = false;

			for (unsigned int i = 0; i < count; i++) {
				Ball ball;
				ball.pos = {
					2.0f * distribution(engine) * radius - radius,
					radius + distribution(engine) * 6.0f - 3.0f,
					2.0f * distribution(engine) * radius - radius
				};

				material.diffuse = {
					0.2f + distribution(engine) * 0.4f,
					0.2f + distribution(engine) * 0.4f,
					0.2f + distribution(engine) * 0.4f
				};

				material.ambient = material.diffuse;
				material.ambient.x /= 0.25f;
				material.ambient.y /= 0.25f;
				material.ambient.z /= 0.25f;

				material.shininess = 20 + distribution(engine) * 180.0f;

				ball.material = material;
				ball.radius = 0.5f;
				ball.velocity = {
					distribution(engine) * 10.0f - 5.0f,
					distribution(engine) * 10.0f - 5.0f,
					distribution(engine) * 10.0f - 5.0f
				};

				balls.push_back(ball);
			}
			waitingDuration = 0;

			t1 = new std::thread(updateBalls, &balls, &radius, &conservationV, &conservationH, &conservationB, &gravity, &isAdded);
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			isAdded = true;
			t1->join();
			delete t1;
			isAdded = false;
			balls.clear();
			t1 = new std::thread(updateBalls, &balls, &radius, &conservationV, &conservationH, &conservationB, &gravity, &isAdded);

		}

		if (handler.getKeyState(GLFW_KEY_F)) {
			isAdded = true;
			t1->join();
			delete t1;
			isAdded = false;

			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({ -ball.pos.x,0,-ball.pos.z });
				ball.velocity.x += direction.z * 3.0f;
				ball.velocity.z -= direction.x * 3.0f;
			}

			t1 = new std::thread(updateBalls, &balls, &radius, &conservationV, &conservationH, &conservationB, &gravity, &isAdded);

		}

	}

	void onSuspend() {
	}

	void onResume() {
	}

	void onDestroy() {
	}
};