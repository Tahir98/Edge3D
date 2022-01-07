#include "Edge3D.h"

#include "imgui.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "glad/glad.h"
#include <map>

#include <vector>


using namespace Edge3D;

enum class GameState {
	START,GAME_LOOP,GAME_OVER
};

class FlappyBird : public Scene {
private:

	InputHandler handler;
	SpriteBatch batch;

	std::vector<Texture*> bird;
	std::vector<Texture*> bee;
	Texture background;

	float gravity = -Application::height / 1200.0f;
	float impulse = Application::height / 60.0f;

	Vec2 birdPos;
	Vec2 birdSize;
	float birdVelocity = 0;
	float birdAnimSpeed = 25;
	float counter1 = 0, counter2 = 0;
	bool isClick = false;

	Vec2 beeSize;
	Vec2 beePos[4 * 3];
	bool isPassed[4] = { 0 };

	float beeAnimSpeed = 20;
	float beeSpeed = Application::height/120.0f;

	Vec2 backPos;
	Vec2 backSize;
	float backVelocity = Application::height / 300.0f;

	int pos[7] = { 1,2,3,4,5,6,7 };
	int size = 7; 

	unsigned int score = 0;
	bool isCollide = false;

	ShapeRenderer2D shape;

	GameState state = GameState::START;

public:

	FlappyBird() : Scene("Flappy Bird"), handler(), background("assets/city.png") ,batch(Application::width,Application::height), shape(Application::width, Application::height) {
		bird.reserve(4);
		bee.reserve(2);
		
		bird.push_back(new Texture("assets/bird/bird1.png"));
		bird.push_back(new Texture("assets/bird/bird2.png"));
		bird.push_back(new Texture("assets/bird/bird3.png"));
		bird.push_back(new Texture("assets/bird/bird4.png"));
		
		bee.push_back(new Texture("assets/bee/bee1.png"));
		bee.push_back(new Texture("assets/bee/bee2.png"));
		
		srand(time(NULL));
		
		birdSize = { Application::height / 12,Application::height / 12 };
		birdPos = { Application::width / 4, Application::height / 2.0f - birdSize.y / 2.0f };

		beeSize = { Application::height / 6.0f,Application::height / 8.0f };
		for (int i = 0; i < 4; i++) {			
			reset();
			beePos[i * 3] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
			beePos[i * 3 + 1] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
			beePos[i * 3 + 2] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
		}

		for (int i = 0; i < 12; i++) {
			std::cout << "x: " << beePos[i].x << "  y: " << beePos[i].y << std::endl;
		}

		backPos = { 0,0 };
		backSize = { Application::height * 1.0f / Application::aspectRatio,Application::height };

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		shape.setColor({1,0,0,1});

	}

	~FlappyBird() {
		for (unsigned int i = 0; i < 4; i++) {
			delete bird[i];
		}

		for (unsigned int i = 0; i < 2; i++) {
			delete bee[i];
		}
	}
	
	void onCreate() override {

	}

	void onUpdate(const float delta) override {
		input(delta);
		imguiRender(delta);

		if (state == GameState::GAME_LOOP) {
			collisionDetection();
		}

		if (state == GameState::GAME_LOOP) {
			counter1 += delta * birdAnimSpeed;
			if (counter1 >= 4.0f)
				counter1 = counter1 - (int)counter1;

			counter2 += delta * beeAnimSpeed;
			if (counter2 >= 2.0f)
				counter2 = counter2 - (int)counter2;

			backPos.x -= backVelocity * delta * 60.0f;
			if (backPos.x <= -backSize.x) {
				backPos.x += backSize.x;
			}

			birdVelocity += gravity * delta * 60.0f;
			birdPos.y += birdVelocity * delta * 60.0f;
			if (birdPos.y <= Application::height / 8.0f) {
				birdPos.y = Application::height / 8.0f;
				birdVelocity = 0;
				state = GameState::GAME_OVER;
			}
			else if (birdPos.y >= Application::height - birdSize.y) {
				birdPos.y = Application::height - birdSize.y;
				birdVelocity *= -0.1f;
			}

			for (int i = 0; i < 4; i++) {
				beePos[i * 3].x -= beeSpeed * delta * 60.0f;
				beePos[i * 3 + 1].x -= beeSpeed * delta * 60.0f;
				beePos[i * 3 + 2].x -= beeSpeed * delta * 60.0f;

				if (beePos[i * 3].x <= -beeSize.x) {
					beePos[i * 3].x += 4 * Application::height;
					beePos[i * 3 + 1].x += 4 * Application::height;
					beePos[i * 3 + 2].x += 4 * Application::height;

					isPassed[i] = false;

					reset();
					beePos[i * 3].y = getRandomLocation() * Application::height / 8;
					beePos[i * 3 + 1].y = getRandomLocation() * Application::height / 8;
					beePos[i * 3 + 2].y = getRandomLocation() * Application::height / 8;
				}
			}

			for (unsigned int i = 0; i < 4; i++) {
				if (beePos[i * 3].x + beeSize.x <= birdPos.x && !isPassed[i]) {
					isPassed[i] = true;
					score++;
				}
			}
		}
		
		

		if (state == GameState::GAME_LOOP && isCollide) {
			state = GameState::GAME_OVER;
		}

		render(delta);
		
	}
	
	
	void input(const float delta) {
		if (state == GameState::GAME_LOOP) {
			if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
				if (!isClick) {
					isClick = true;
					birdVelocity = impulse;
				}
			}
			else {
				isClick = false;
			}
		}
		else if (state == GameState::GAME_OVER) {
			if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
				if (!isClick) {
					isClick = true;
					birdVelocity = impulse;
					state = GameState::GAME_LOOP;
					resetPos();
				}
			}
			else {
				isClick = false;
			}
		}
		else if (state == GameState::START) {
			if (handler.getMouseButtonState(GLFW_MOUSE_BUTTON_LEFT)) {
				if (!isClick) {
					isClick = true;
					birdVelocity = impulse;
					state = GameState::GAME_LOOP;
					resetPos();
				}
			}
			else {
				isClick = false;
			}
		}
		
		
	}

	
	
	void render(float delta) {
		batch.begin();
		batch.draw(background, backPos.x, backPos.y, Application::height * 1.0f / Application::aspectRatio, Application::height);
		batch.draw(background, backPos.x + backSize.x, backPos.y, Application::height * 1.0f / Application::aspectRatio, Application::height);
		batch.draw(background, backPos.x + 2.0f * backSize.x, backPos.y, Application::height * 1.0f / Application::aspectRatio, Application::height);

		if (state == GameState::START || state == GameState::GAME_LOOP) {
			batch.draw(*bird[(int)counter1], birdPos.x, birdPos.y, birdSize.x, birdSize.y);

			for (unsigned int i = 0; i < 4; i++) {
				batch.draw(*bee[(int)counter2], beePos[i * 3].x, beePos[i * 3].y, beeSize.x, beeSize.y);
				batch.draw(*bee[(int)counter2], beePos[i * 3 + 1].x, beePos[i * 3 + 1].y, beeSize.x, beeSize.y);
				batch.draw(*bee[(int)counter2], beePos[i * 3 + 2].x, beePos[i * 3 + 2].y, beeSize.x, beeSize.y);
			}
		}
		
		batch.end();
	

		/*shape.begin(ShapeType::LINE);

		shape.circle(birdPos.x + birdSize.x / 2.0f, birdPos.y + birdSize.x / 2.0f, birdSize.x / 2.0f);

		for (unsigned int i = 0; i < 12; i++) {
			shape.ellipse(beePos[i].x + beeSize.x / 2.0f, beePos[i].y + beeSize.y / 2.0f, beeSize.x / 2.0f - Application::height / 40.0f, beeSize.y / 2.0f - Application::height / 40.0f);
		}

		shape.end();*/
	}



	void collisionDetection() {
		float a = beeSize.x / 2.0f - Application::height / 100.0f;
		float b = beeSize.y / 2.0f - Application::height / 100.0f;

		isCollide = false;

		for (unsigned int i = 0; i < 12; i++) {
			Vec2 diff = (beePos[i] + beeSize * 0.5f)- (birdPos + birdSize * 0.5f);
			float length = diff.length();
			
			float radius = sqrtf(a*a * b*b /(a*a - (a*a - b*b)*cosf(atan2f(diff.y,diff.x))));
			if (radius + birdSize.x / 2.0f > length) {
				isCollide = true;
				break;
			}
		}
	}

	void resetPos() {
		birdPos = { Application::width / 4, Application::height / 2.0f - birdSize.y / 2.0f };

		for (int i = 0; i < 4; i++) {
			reset();
			beePos[i * 3] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
			beePos[i * 3 + 1] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
			beePos[i * 3 + 2] = { Application::width + i * Application::height,getRandomLocation() * Application::height / 8 };
		}

		score = 0;
	}

	void imguiRender(float delta) {
		ImGui::Begin("Settings");
		ImGui::Text("FPS: %.3f  Frame Time: %.4f", 1.0f / delta, delta);
		ImGui::SliderFloat("Animation Speed",&birdAnimSpeed,0,60);
		ImGui::End();
	}

	int getRandomLocation() {
		int loc = rand() % size;
		int value = pos[loc];

		for (int i = loc; i < size - 1; i++) {
			pos[i] = pos[i + 1];
		}

		size--;

		return value;
	}

	void reset() {
		for (int i = 0; i < 7; i++) {
			pos[i] = i + 1;
		}

		size = 7;
	}

	void onSuspend() override {
	}

	void onResume() override {

	}

	void onDestroy() override {
	
	}
};