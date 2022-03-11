#pragma once

#include "Edge3D.h"

class PhysicsEngine : public Edge3D::Scene {
private:
	struct Rect {
		Vec2 pos;
		Vec2 size;
		Vec4 color;
	};

	struct Sphere {
		Vec2 pos;
		float radius;
		float mass;
		Vec2 velocity;
		Vec2 acceleration;
		float aVelocity;
		float aAcceleration;
		float orientation;
		Vec4 color;
	};

	Edge3D::Window& window;
	Edge3D::FrameBuffer* fb;
	Edge3D::InputHandler handler;

	Edge3D::ShapeRenderer2D renderer;

	std::vector<Rect> rects;
	std::vector<Sphere> spheres;
	MersenneTwister mt;

	float wWidth, wHeight;
	float gravity = -300.0f;
	float EC_Ground = 0.5f;
	float EC_Wall = 0.5f;
	float EC_Ball = 0.99f;

	float radiusMin = 40;
	float radiusRange = 10;
	float waitingDuration = 0.1f;
	float wait = 0;
public:
	PhysicsEngine(Edge3D::Window& window, std::string name) : window(window), Scene(name), renderer(window.getWidth(),window.getHeight()) , mt(true){
		fb = new Edge3D::FrameBuffer(600, 600);

		for (int i = 0; i < 50; i++) {
			addBall();
		}

		
	}

	~PhysicsEngine() {
		delete fb;
	}

	void onCreate() {

	}

	void onUpdate(float delta) {
		input(delta);
		updateSpheres(delta);

		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);

		renderer.begin(Edge3D::ShapeType::FILLED);

		for (int i = 0; i < spheres.size(); i++) {
			Sphere& s = spheres[i];
			renderer.setColor(s.color);
			renderer.circle(s.pos.x, s.pos.y, s.radius);
			renderer.setColor(1, 1, 1, 1);
		}
		renderer.end();

		renderer.begin(Edge3D::ShapeType::LINE);

		glLineWidth(4);

		for (int i = 0; i < spheres.size(); i++) {
			Sphere& s = spheres[i];

			Vec2 p1 = RotateAround(s.pos - Vec2{0, s.radius}, s.pos, s.orientation);

			renderer.line(s.pos.x, s.pos.y, p1.x, p1.y);
		}

		renderer.end();
		
		fb->unbind();

		wWidth = fb->getWidth();
		wHeight = fb->getHeight();

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
		if (handler.getKeyState(GLFW_KEY_UP)) {
			for (Sphere& s : spheres) {
				s.velocity = s.velocity + Vec2{0,450} * delta;
			}
		}
		else if (handler.getKeyState(GLFW_KEY_DOWN)) {
			for (Sphere& s : spheres) {
				s.velocity = s.velocity + Vec2{ 0,-150 } * delta;
			}
		}

		if (handler.getKeyState(GLFW_KEY_RIGHT)) {
			for (Sphere& s : spheres) {
				s.velocity = s.velocity + Vec2{ 450,0 } * delta;
			}
		}
		else if (handler.getKeyState(GLFW_KEY_LEFT)) {
			for (Sphere& s : spheres) {
				s.velocity = s.velocity + Vec2{ -450,0 } * delta;
			}
		}

		wait += delta;

		if (handler.getKeyState(GLFW_KEY_F)) {
			for (int i = 0; i < 1; i++) {
				if (wait >= waitingDuration) {
					addBall();
					wait = 0;
				}

			}
		}

		if (handler.getKeyState(GLFW_KEY_C)) {
			spheres.clear();
			spheres.shrink_to_fit();
		}

		ImGui::Begin("Settings");
		ImGui::SliderFloat("Gravity", &gravity, -100, 0);
		ImGui::NewLine();

		ImGui::Separator();
		ImGui::Text("Energy Conservation");
		ImGui::SliderFloat("Ground", &EC_Ground, 0, 2);
		ImGui::SliderFloat("Wall", &EC_Wall, 0, 2);
		ImGui::SliderFloat("Ball", &EC_Ball, 0, 2);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Spawn Properties");
		ImGui::SliderFloat("Radius Min", &radiusMin, 1, 100);
		ImGui::SliderFloat("Random Range", &radiusRange, 0, 100);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Number of balls: %d", spheres.size());

		ImGui::End();
	}

	void updateSpheres(float delta) {
		for (int i = 0; i < spheres.size(); i++) {
			Sphere& s1 = spheres[i];
			s1.velocity.y += gravity * delta;
			s1.pos = s1.pos + s1.velocity * delta;
			s1.orientation += s1.aVelocity * delta;

			if (s1.pos.y <= s1.radius) {
				s1.pos.y = s1.radius + (s1.radius - s1.pos.y);
				s1.velocity.y = -s1.velocity.y * EC_Ground;
			}
			else if (s1.pos.y + s1.radius >= wHeight) {
				s1.pos.y = wHeight - s1.radius - (s1.pos.y + s1.radius - wHeight);
				s1.velocity.y = -s1.velocity.y * EC_Ground;
			}

			if (s1.pos.x - s1.radius <= 0) {
				s1.pos.x = s1.radius + (s1.radius - s1.pos.x);
				s1.velocity.x = -s1.velocity.x * EC_Wall;
			}
			else if (s1.pos.x + s1.radius >= wWidth) {
				s1.pos.x = s1.pos.x  - (s1.pos.x + s1.radius - wWidth);
				s1.velocity.x = -s1.velocity.x * EC_Wall;
			}

			for (int j = i+1; j < spheres.size(); j++) {
				Sphere& s2 = spheres[j];

				float length = (s1.pos - s2.pos).length();

				if (length < s1.radius + s2.radius) {
					float diff = s1.radius + s2.radius - length;

					Vec2 direction = Math::normalize(s2.pos - s1.pos);

					s1.pos = s1.pos - direction * diff * 0.50f;
					s2.pos = s2.pos + direction * diff * 0.50f;

					
					Vec2 vel1 = s1.velocity + Vec2{-direction.y * s1.aVelocity, direction.x * s1.aVelocity};
					Vec2 vel2 = s2.velocity + Vec2{ direction.y * s2.aVelocity, -direction.x * s2.aVelocity };

					Vec2 newVel1 = (vel1 - (2.0f * s2.mass /(s1.mass + s2.mass)) * (Math::dotProduct(vel1 - vel2, s1.pos - s2.pos) / (s1.pos - s2.pos).lengthSquare()) * (s1.pos - s2.pos)) * EC_Ball;

					Vec2 newVel2 = (vel2 - (2.0f * s1.mass / (s1.mass + s2.mass)) * (Math::dotProduct(vel2 - vel1, s2.pos - s1.pos) / (s2.pos - s1.pos).lengthSquare()) * (s2.pos - s1.pos)) * EC_Ball;

					s1.velocity = Math::dotProduct(newVel1, 1.0f * direction) * 1.0f * direction;
					s1.aVelocity = Math::dotProduct(newVel1, Vec2{ -direction.y, direction.x});

					s2.velocity = Math::dotProduct(newVel2, -1.0f * direction) * -1.0f * direction;
					s2.aVelocity = Math::dotProduct(newVel2, Vec2{ direction.y, -direction.x });
				}
			}
		}
	}


	void addBall() {
		Sphere sphere;
		sphere.pos = { (float)mt.nextUInt(100,1800),(float)mt.nextUInt(100,900) };
		sphere.radius = radiusMin + mt.nextFloat() * radiusRange;
		sphere.mass = sphere.radius * sphere.radius;
		sphere.velocity = { 0,0 };
		sphere.acceleration = { 0,0 };
		sphere.aVelocity = 0;
		sphere.aAcceleration = 0;
		sphere.orientation = 0;
		sphere.color = { mt.nextFloat(),mt.nextFloat(),mt.nextFloat(),1.0f };
		spheres.push_back(sphere);
	}

	Vec2 RotateAround(Vec2 point, Vec2 origin,float degree) {

		Vec2 vec2 = point - origin;
		Vec2 p;

		p.x = cosf(Math::toRadian(degree)) * vec2.x - sinf(Math::toRadian(degree)) * vec2.y;
		p.y = sinf(Math::toRadian(degree)) * vec2.x + cosf(Math::toRadian(degree)) * vec2.y;

		return origin + p;
	}
};