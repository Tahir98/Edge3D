#pragma once

#include <Edge3D.h>
#include <vector>
#include <stdio.h>
#include "imgui.h"
#include <iostream>

using namespace Edge3D;

struct Particle {
	float x,y;
	float radius;
	Vec2 velocity;
	float mass;
	Vec3 color;
};

class Physics : public Scene {
private:
	GLFWwindow* window;
	InputHandler handler;
	ShapeRenderer2D renderer;

	std::vector<Particle> particles;

	float gravityY = -0.6f;
	float gravityX =  0.0f;

	int segment = 40;

	float momentum = 0.9f;
	int chosen = -1;
	float line[4];

	
public:

	Physics(GLFWwindow* window) :Scene("Physics Demo"), window(window), renderer(Application::width, Application::height){
	
	}
	
	void onCreate() override {

	}

	void onUpdate(const float delta) override {
		input(delta);

		for (unsigned int i = 0; i < particles.size(); i++) {
			Particle& particle = particles[i];

			particle.velocity.y += gravityY * delta * 60.0f;
			particle.y += particle.velocity.y * delta * 60.0f;

			particle.velocity.x += gravityX * delta * 60.0f;
			particle.x += particle.velocity.x * delta * 60.0f;

			//Collision width walls
			if (particle.y <= particle.radius) {
				particle.y = 2.0f * particle.radius - particle.y;
				particle.velocity.y *= -0.9f;
			}
			else if (particle.y + particle.radius >= Application::height) {
				particle.y = 2.0f * Application::height - 2.0f * particle.radius - particle.y;
				particle.velocity.y *= -0.9f;
			}

			if (particle.x <= particle.radius) {
				particle.x = 2.0f * particle.radius - particle.x;
				particle.velocity.x *= -0.9f;
			}
			else if (particle.x + particle.radius >= Application::width) {
				particle.x = 2.0f * Application::width - 2.0f * particle.radius - particle.x;
				particle.velocity.x *= -0.9f;
			}

			//Collision with each other
			for (unsigned int j = 0; j < particles.size(); j++) {
				if (j == i)
					continue;
				Particle& particle2 = particles[j];
				float disX = (particle2.x - particle.x);
				float disY = (particle2.y - particle.y);
				if (disX * disX + disY * disY <= (particle.radius +			particle2.radius) * (particle.radius + particle2.radius)) {
					float distance = sqrtf(disX * disX + disY * disY);
					float overlap = 0.5f * (distance - particle.radius - particle2.radius);

					float totalMass = particle.mass + particle2.mass;
					particle.x -= overlap * (particle.x - particle2.x) / distance;
					particle.y -= overlap * (particle.y - particle2.y) / distance;

					particle2.x += overlap * (particle.x - particle2.x) / distance;
					particle2.y += overlap * (particle.y - particle2.y) / distance;

					disX = (particle2.x - particle.x);
					disY = (particle2.y - particle.y); 
					distance = sqrtf(disX * disX + disY * disY);
					float nx = disX / distance;
					float ny = disY / distance;

					float kx = particle.velocity.x - particle2.velocity.x;
					float ky = particle.velocity.y - particle2.velocity.y;

					float p = 2.0f * (nx * kx + ny * ky) / (particle.mass + particle2.mass);

					particle.velocity.x = (particle.velocity.x - p * particle2.mass * nx) * momentum;
					particle.velocity.y = (particle.velocity.y - p * particle2.mass * ny) * momentum;

					particle2.velocity.x = (particle2.velocity.x + p * particle.mass * nx) * momentum;
					particle2.velocity.y = (particle2.velocity.y + p * particle.mass * ny) * momentum;

				}				
			}
		}

		imguiDraw();

		


		renderer.begin(ShapeType::FILLED);
		
		for (const auto& particle : particles) {
			renderer.setColor(particle.color.x, particle.color.y, particle.color.z,1);
			renderer.circle(particle.x, particle.y, particle.radius, segment);
		}

		renderer.end();



		if (chosen != -1) {
			renderer.begin(ShapeType::LINE);
			renderer.setColor({0,0,1,1});
			renderer.line(line[0], line[1], line[2], line[3]);
			renderer.end();
		}
		
			
		

		std::string size = std::to_string(particles.size());
	}


	void input(const float delta) {
		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2)) {
			double x, y;
			glfwGetCursorPos(window,&x,&y);

			y = Application::height - y;
			
			bool isEmpty = true;
			for (unsigned int i = 0; i < particles.size();i++) {
				Particle particle = particles[i];
				float diffX = x - particle.x;
				float diffY = y - particle.y;
				if (x + particle.radius > Application::width || x - particle.radius < 0) {
					isEmpty = false;
					break;
				}

				if (y + particle.radius > Application::height || y - particle.radius < 0) {
					isEmpty = false;
					break;
				}

				if (diffX * diffX + diffY * diffY <= 2 * particle.radius * 2 * particle.radius) {
					isEmpty = false;
					break;
				}
			}

			if (isEmpty) {
				for (unsigned int i = 0; i < 1; i++) {
					Particle particle;
					particle.x = (float)x;
					particle.y = (float)y;
					particle.radius = 20 + rand() % 60;
					particle.mass = particle.radius * particle.radius;
					particle.velocity = { (float)(rand() % 20 - 10),(float)(rand() % 20 - 10) };
					particle.color = { (float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f,(float)(rand() % 256) / 256.0f };

			
					particles.push_back(particle);
				}
			}
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			particles.clear();
		}

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_1)) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			y = Application::height - y;

			if (chosen == -1) {
				for (unsigned int i = 0; i < particles.size(); i++) {
					float diffX = x - particles[i].x;
					float diffY = y - particles[i].y;
					float distance = diffX * diffX + diffY * diffY;
					if (distance < particles[i].radius * particles[i].radius) {
						chosen = i;
						line[0] = x;
						line[1] = y;
						line[2] = particles[i].x;
						line[3] = particles[i].y;
						break;
					}
				}
			}
			else {
				line[0] = x;
				line[1] = y;
				line[2] = particles[chosen].x;
				line[3] = particles[chosen].y;
			}
		}else if(chosen != -1){
			particles[chosen].velocity.x += (line[2] - line[0]) / 5.0f;
			particles[chosen].velocity.y += (line[3] - line[1]) / 5.0f;
			chosen = -1;
		}
	}

	void imguiDraw() {
		ImGui::Begin("Settings");
		ImGui::SliderFloat("gravityY", &gravityY, -5, 5);
		ImGui::SliderFloat("gravityX", &gravityX, -5, 5);
		ImGui::SliderInt("Segment",&segment,3,50);
		ImGui::SliderFloat("Momentum conservation", &momentum, 0, 1);
		ImGui::End();
	}

	void onSuspend() override{

	}

	void onResume() override {

	}

	void onDestroy() override {
	
	}
};