#pragma once
#include "Edge3D.h"

class PhysicsSimulation2D : public Edge3D::Scene {
private:
	Edge3D::Window& window;
	Edge3D::FrameBuffer* fb = nullptr;

	Edge3D::InputHandler handler;

	Edge3D::ShapeRenderer2D renderer;
	ImVec2 fb_size = { 1920,1080 };

	float gravity = -500;

	struct Circle {
		Vec2 pos;
		float radius = 60;
		Vec4 color = { 1,0,0,1 };
		Vec2 velocity;
		float angular_velocity = 120;
		float rotation = 0;
	};

	std::vector<Circle> circles;
	Xorshift xorshift;

	float edge_bounce = 0.9f;
	float circle_bounce = 0.9f;
	float input_force = 900;

	int circleCount = 10;
	float waitingduration = 1;

	bool rightClicked = false;
	int selected = -1;
	Vec2 cursorPos;

public:
	PhysicsSimulation2D(Edge3D::Window& window, std::string name) : window(window), Scene(name), renderer(fb_size.x,fb_size.y) {

	}

	~PhysicsSimulation2D() {
		delete fb;
	}

	void onCreate() {
		fb = new Edge3D::FrameBuffer(fb_size.x,fb_size.y);

		for (unsigned int i = 0; i < 20; i++) {
			Circle circle;
			circle.pos = { circle.radius + xorshift.nextFloat() * (fb_size.x - 2.0f * circle.radius),
			circle.radius + xorshift.nextFloat() * (fb_size.y - 2.0f * circle.radius) };
			circle.velocity = { xorshift.nextFloat() * 200.0f - 100.0f, xorshift.nextFloat() * 200.0f - 100.0f };

			circle.color.x = xorshift.nextFloat() * 0.7f;
			circle.color.y = xorshift.nextFloat() * 0.7f;
			circle.color.z = xorshift.nextFloat() * 0.7f;

			//APP_LOG_ERROR("Circle1 color: {0},{1},{2}", circle.color.x, circle.color.y, circle.color.z);
			
			circles.push_back(circle);
		}
	}

	void onUpdate(float delta) {
		inputControl(delta);
		updateCircles(delta);

		fb->bind();
		fb->clear();
		fb->clearColor(0,0,0,1);

		renderer.begin(Edge3D::ShapeType::FILLED);
		
		for (auto& circle : circles) {
			renderer.setColor(circle.color);
			renderer.circle(circle.pos.x,circle.pos.y,circle.radius);
			renderer.setColor(1, 1, 1, 1);
			//renderer.polygon(circle.pos.x, circle.pos.y, circle.radius - 5, 3, circle.rotation);
		}

		renderer.end();

		if (selected != -1) {
			renderer.begin(Edge3D::ShapeType::LINE);

			renderer.setColor(1, 0, 0, 1);
			renderer.line(cursorPos.x, cursorPos.y, circles[selected].pos.x, circles[selected].pos.y);

			renderer.end();
		}

		fb->unbind();

		settingPanel(delta);

		ImGui::Begin("Simulation");

		ImVec2 avail_size = ImGui::GetContentRegionAvail();
		ImVec2 avail_max = ImGui::GetContentRegionMax();

		if (fb_size.x != avail_size.x || fb_size.y != avail_size.y) {
			fb_size = avail_size;
			ImVec2 size = ImGui::GetWindowSize();
			fb->resize(avail_size.x, avail_size.y);
			renderer.setWorldSize(avail_size.x, avail_size.x * (size.y / size.x));
	

			APP_LOG_WARN("Available Window size: {0},{1}", avail_size.x, avail_size.y);
			APP_LOG_WARN("Maximum Available Window size: {0},{1}", avail_max.x, avail_max.y);
		}

		ImGui::Image((ImTextureID)fb->getTextureID(), avail_size, ImVec2(0, 1), ImVec2(1, 0));
		

		ImGui::End();
	}

	void inputControl(float delta) {
		if (handler.getKeyState(GLFW_KEY_A)) {
			for (auto& circle : circles) {
				circle.velocity.x -= input_force * delta;
			}
		}

		if (handler.getKeyState(GLFW_KEY_D)) {
			for (auto& circle : circles) {
				circle.velocity.x += input_force * delta;
			}
		}

		if (handler.getKeyState(GLFW_KEY_W)) {
			for (auto& circle : circles) {
				circle.velocity.y += input_force * delta;
			}
		}

		if (handler.getKeyState(GLFW_KEY_S)) {
			for (auto& circle : circles) {
				circle.velocity.y -= input_force * delta;
			}
		}

		waitingduration += delta;

		if (handler.getKeyState(GLFW_KEY_F) && waitingduration >= 1.0f) {
			addCircle();
			waitingduration = 0;
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			circles.clear();
			circles.shrink_to_fit();
		}

		if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && !rightClicked) {
			rightClicked = true;

			cursorPos = handler.getMousePos();
			cursorPos.y = window.getHeight() - cursorPos.y;

			for (uint32_t i = 0; i < circles.size(); i++) {
				if ((cursorPos - circles[i].pos).length() < circles[i].radius) {
					selected = i;
					break;
				}
			}
		}
		else if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && rightClicked) {
			cursorPos = handler.getMousePos();
			cursorPos.y = window.getHeight() - cursorPos.y;
		}
		else if (!handler.getMouseButtonState(GLFW_MOUSE_BUTTON_2) && rightClicked) {
			if (selected != -1) {

				Vec2 direction = circles[selected].pos - cursorPos;
				float length = direction.length();
				direction = direction * (1.0f / length);

				circles[selected].velocity = circles[selected].velocity + direction * (length * 8.0f);
			}	

			rightClicked = false;
			selected = -1;
		}
	}

	void settingPanel(float delta) {
		ImGui::Begin("Settings");
		ImGui::Text("Frame time: %.2fms",delta);
		ImGui::Text("Number of Balls: %d", circles.size());
		ImGui::SliderFloat("Edge Bounce", &edge_bounce, 0, 2);
		ImGui::SliderFloat("Circle Bounce", &circle_bounce, 0, 2);
		ImGui::SliderInt("Add", &circleCount, 1, 100);
		ImGui::End();
	}

	void updateCircles(float delta) {
		for (uint32_t i = 0; i < circles.size(); i++) {
			Circle& circle1 = circles[i];

			circle1.velocity.y += gravity * delta;
			circle1.pos = circle1.pos + circle1.velocity * delta;
			circle1.rotation += circle1.angular_velocity * delta;

			if (circle1.pos.y - circle1.radius <= 0) {
				circle1.velocity.y *= -edge_bounce;
				circle1.pos.y = circle1.radius - circle1.pos.y + circle1.radius;
			}

			if (circle1.pos.y + circle1.radius >= fb_size.y) {
				circle1.velocity.y *= -edge_bounce;
				circle1.pos.y = fb_size.y - circle1.radius - (circle1.pos.y + circle1.radius - fb_size.y);
			}

			if (circle1.pos.x - circle1.radius <= 0) {
				circle1.velocity.x *= -edge_bounce;
				circle1.pos.x = circle1.radius - circle1.pos.x + circle1.radius;
			}

			if (circle1.pos.x + circle1.radius >= fb_size.x) {
				circle1.velocity.x *= -edge_bounce;
				circle1.pos.x = fb_size.x - circle1.radius - (circle1.pos.x + circle1.radius - fb_size.x);
			}

			for (uint32_t j = i + 1; j < circles.size(); j++) {
				Circle& circle2 = circles[j];

				float distance = 0;
				if (circle1.radius + circle2.radius >= (distance = (circle1.pos - circle2.pos).length())) {
					float diff = circle1.radius + circle2.radius - distance;
					Vec2 direction = Math::normalize(circle1.pos - circle2.pos);
					Vec2 normal = Vec2{ direction.y, -direction.x };
				
					//Seperating circles from each other
					circle1.pos = circle1.pos + direction * diff * 0.51f;
					circle2.pos = circle2.pos - direction * diff * 0.51f;

					//Vec2 rel_velocity = circle1.velocity - circle2.velocity; // relative velocity;
					//
					//circle1.velocity = normal * Math::dotProduct(rel_velocity * -1.0f, normal) * circle_bounce;
					//circle2.velocity = (direction * -1.0f) * Math::dotProduct(rel_velocity, direction * -1.0f) * circle_bounce;
					//
					

					Vec2 temp = circle1.velocity;
					
					circle1.velocity = circle1.velocity - Math::dotProduct(circle1.velocity - circle2.velocity, circle1.pos - circle2.pos) *
						(circle1.pos - circle2.pos) * (1.0f / (circle1.pos - circle2.pos).lengthSquare());
					
					circle2.velocity = circle2.velocity - Math::dotProduct(circle2.velocity - temp, circle2.pos - circle1.pos) *
						(circle2.pos - circle1.pos) * (1.0f / (circle2.pos - circle1.pos).lengthSquare());
				}
			}
		}
	}

	void addCircle() {
		for (unsigned int i = 0; i < circleCount; i++) {
			Circle circle;
			circle.pos = { circle.radius + xorshift.nextFloat() * (fb_size.x - 2.0f * circle.radius),
			circle.radius + xorshift.nextFloat() * (fb_size.y - 2.0f * circle.radius) };
			circle.velocity = { xorshift.nextFloat() * 200.0f - 100.0f, xorshift.nextFloat() * 200.0f - 100.0f };

			circle.color.x = xorshift.nextFloat() * 0.7f;
			circle.color.y = xorshift.nextFloat() * 0.7f;
			circle.color.z = xorshift.nextFloat() * 0.7f;

			//APP_LOG_ERROR("Circle1 color: {0},{1},{2}", circle.color.x, circle.color.y, circle.color.z);

			circles.push_back(circle);
		}
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
};