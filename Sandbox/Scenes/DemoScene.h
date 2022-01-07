#pragma once
#include "Edge3D.h"
#include <vector>
#include <random>
#include <chrono>

#include <thread>

static float frametime;

class DemoScene : public Edge3D::Scene {
private:
	struct Ball {
		Vec3 pos;
		Vec3 velocity;
		Material material;
		float radius;
	};

	Edge3D::Window& window;

	float width;
	float height = 100;

	Light light;
	Material lightMaterial;
	Edge3D::Sphere sphere;
	Edge3D::Plane plane;
	Edge3D::PerspectiveCamera camera;
	Edge3D::SpherecalSkybox skybox;
	int segment = 20;

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

	bool stop = false;
	std::thread* t1 = nullptr;

	bool showPanel = true;
	bool wireframe = false;
public:
	DemoScene(Edge3D::Window& window) : Edge3D::Scene("DemoScene") , window(window), camera(90 * window.getAspectRatio(),window.getAspectRatio(),0.1f,300), skybox("assets/forest_slope.jpg"), distribution(0,1),sphere("assets/ac.jpg"){

		width = 16;

		gravity = -10.0f;
		conservationV = 0.7f; conservationH = 0.7f;
		conservationB = 0.99f;
		 
		t1 = new std::thread(updateBalls, &balls,&width,&height, &conservationV, &conservationH, &conservationB, &gravity, &stop);

		sphere.drawTexture = false;
	}

	void onCreate() {
		light.ambient = { 0.3f,0.3f,0.3f };
		light.diffuse = { 0.6f,0.6f,0.6f };
		light.specular = { 0.9f,0.9f,0.9f };
		light.pos = {0,10,0};
	
		lightMaterial.ambient = { 0.3f,0.0f,0.0f };
		lightMaterial.diffuse = { 0.6f,0.0f,0.0f };
		lightMaterial.specular = { 0.0f,0.0f,0.0f };
		lightMaterial.shininess = 100;

		Material material;

		material.ambient = { 0.0f,0.0f,0.3f };
		material.diffuse = { 0.0f,0.0f,0.9f };
		material.specular = { 0.0f,0.0f,0.0f };
		material.shininess = 100;

		plane.setMaterial(material);

		sphere.setSegment(10);
	}

	void onUpdate(float delta) {
		input(delta);
		camera.update(delta);
		
		if (wireframe) {
			sphere.setDrawMode(Edge3D::Shape::DrawMode::LINE);
		}
		else {
			sphere.setDrawMode(Edge3D::Shape::DrawMode::TRIANGLE);
		}

		if (sphere.getSegment() != segment) {
			sphere.setSegment(segment);
		}

		skybox.draw(camera);

		sphere.setPosition(light.pos);
		sphere.setMaterial(lightMaterial);
		sphere.draw(camera);

		//plane.setRotation({0,0,0});
		//plane.setSize({2.0f * width,2.0f * width,2.0f * width });
		//plane.setPosition({0,0,0});
		//plane.draw(camera, light);
		//
		//plane.setRotation({ 0,0,PI / 2.0f });
		//plane.setSize({ width,2.0f * width,2.0f * width });
		//plane.setPosition({ width,width/2.0f,0 });
		//plane.draw(camera, light);
		//
		//plane.setRotation({ 0,0,-PI / 2.0f });
		//plane.setSize({ width,2.0f * width,2.0f * width });
		//plane.setPosition({ -width,width/2.0f,0 });
		//plane.draw(camera, light);
		//
		//plane.setRotation({ PI / 2.0f,0,0 });
		//plane.setSize({ 2.0f * width,2.0f * width,width });
		//plane.setPosition({ 0,width/2.0f,width });
		//plane.draw(camera, light);
		//
		//plane.setRotation({ -PI / 2.0f,0,0 });
		//plane.setSize({ 2.0f * width,2.0f * width,width });
		//plane.setPosition({ 0,width / 2.0f,-width });
		//plane.draw(camera, light);	

		for (Ball& ball : balls) {
			sphere.setPosition(ball.pos);
			sphere.setMaterial(ball.material);
			sphere.draw(camera,light);
		}
		
	}

	static void updateBalls(std::vector<Ball>* balls,float* width,float* height,float* conservationV,float* conservationH,float* conservationB,float* gravity,bool* stop) {
		float delta = 0;
		float prev ,now;
		auto begin = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

		while (!(*stop)) {
			begin = std::chrono::high_resolution_clock::now();
		
			for (unsigned int i = 0; i < balls->size();i++) {
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

	void input(float delta){
		if (showPanel) {
			ImGui::Begin("Profiler");
			ImGui::Text("FPS = %.2f  Frame Time = %.3f", 1.0f / delta, delta);
			ImGui::Text("Number of balls : %d, Thread2 time = %.3f", balls.size(), frametime);

			ImGui::SliderInt("Segment", &segment, 2, 50);
			ImGui::Checkbox("Wireframe",&wireframe);

			ImGui::Text("\n");

			ImGui::SliderFloat3("Light pos", (float*)&light.pos, -20, 20);
			ImGui::SliderFloat3("Light ambient", (float*)&light.ambient, 0, 1);
			ImGui::SliderFloat3("Light diffuse", (float*)&light.diffuse, 0, 1);
			ImGui::SliderFloat3("Light specular", (float*)&light.specular, 0, 1);

			ImGui::Text("\n");
			ImGui::SliderFloat("Width", &width, 2, 50);
			ImGui::SliderFloat("Height", &height, 10, 300);

			ImGui::Text("\nGround Enegy Conservation");
			ImGui::SliderFloat("V", &conservationV, 0, 2);

			ImGui::Text("\nWalls Enegy Conservation");
			ImGui::SliderFloat("H", &conservationH, 0, 2);

			ImGui::Text("\nCollision Enegy Conservation");
			ImGui::SliderFloat("B", &conservationB, 0, 2);

			ImGui::Text("\nCount");
			ImGui::SliderInt("Count", &count, 2, 500);

			ImGui::Text("\nGravity");
			ImGui::SliderFloat("Gravity", &gravity, -100, -5);

			ImGui::End();
		}
		
		if (handler.getKeyState(GLFW_KEY_T)) {
			showPanel = !showPanel;
		}

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !isClicked) {
			prev = now = handler.getMousePos();
			isClicked = true;

			glfwSetInputMode(window.getGLFWWindowPointer(),GLFW_CURSOR,GLFW_CURSOR_DISABLED);
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
			camera.translate2(0,0, cameraSpeed * delta);
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
			material.specular = {0.9f,0.9f,0.9f};

			for (unsigned int i = 0; i < count; i++) {
				Ball ball;
				ball.pos = { 
					2.0f * distribution(engine) * width - width,
					width + distribution(engine) * 6.0f - 3.0f,
					2.0f * distribution(engine) * width - width 
				};

				material.diffuse = { 
					0.2f + distribution(engine) * 0.5f,
					0.2f + distribution(engine) * 0.5f,
					0.2f + distribution(engine) * 0.5f 
				};

				material.ambient = material.diffuse;
				material.ambient.x /= 1.1f;
				material.ambient.y /= 1.1f;
				material.ambient.z /= 1.1f;

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
			
			t1 = new std::thread(updateBalls, &balls, &width,&height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			stop = true;
			t1->join();
			delete t1;
			stop = false;
			balls.clear();
			t1 = new std::thread(updateBalls, &balls, &width, &height, &conservationV, &conservationH, &conservationB, &gravity, &stop);
		}

		if (handler.getKeyState(GLFW_KEY_E)) {
			//stop = true;
			//t1->join();
			//delete t1;
			//stop = false;
		
			for (Ball& ball : balls) {
				Vec3 direction = Math::normalize({-ball.pos.x,0,-ball.pos.z});
				float length = Vec3({ -ball.pos.x,0,-ball.pos.z }).length();

				ball.velocity.x += direction.z * 0.9f * delta * 60.0f ;					 															
				ball.velocity.z -= direction.x * 0.9f * delta * 60.0f ;
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

	void onSuspend() {
	}

	void onResume() {
	}

	void onDestroy(){
		t1->join();
		delete t1;
	}
};