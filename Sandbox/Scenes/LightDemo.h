#pragma once

#include "Edge3D.h"
#include <vector>
#include <stdlib.h>
#include <time.h>

class LightDemo : public Edge3D::Scene {
private:
	Edge3D::Window& window;
	Edge3D::VertexArray* va = nullptr;
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;

	Edge3D::Shader* shader = nullptr;

	std::vector<Vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<Vec3> positions;

	unsigned int segment = 60;
	float radius = 0.5f;

	Material material;
	Edge3D::InputHandler handler;
	float cameraSpeed = 3.0f;
	Edge3D::PerspectiveCamera camera;
	Vec2 prev, now;
	bool isClicked = false;
	float sensivity = 0.025f;

	Edge3D::SpherecalSkybox* skybox = nullptr;

	Matrix4x4 model;
	std::vector<PointLight> lights;
	DirectionalLight dLight;
	SpotLight sLight;
	Material mat;

public:
	LightDemo(Edge3D::Window& window,std::string name) : window(window),Scene(name),camera(90.0f * 9.0f / 16.0f,9.0f/16.0f,0.1f,1000){

		vb = new Edge3D::VertexBuffer(nullptr, 500000, GL_STATIC_DRAW);
		ib = new Edge3D::IndexBuffer(nullptr, 500000, GL_STATIC_DRAW);
		shader = new Edge3D::Shader("Shaders/ForwardVN.shader");

		skybox = new Edge3D::SpherecalSkybox("assets/market.jpg");

		for (unsigned int i = 0; i < 2 * segment; i++) {
			float degreeY = (float)i * PI / (float)segment;
			float degreeX;
			for (unsigned int j = 0; j < segment + 1; j++) {
				degreeX = (float)j * PI / (float)segment;
				Vec3 pos;
				pos.y = cosf(degreeX);
				pos.x = sinf(degreeY) * sinf(degreeX);
				pos.z = cosf(degreeY) * sinf(degreeX);

				vertices.push_back({ pos.x * radius,pos.y * radius ,pos.z * radius });
				vertices.push_back(pos);
			}
		}

		for (unsigned int i = 0; i < 2 * segment; i++) {
			for (unsigned int j = 0; j < segment; j++) {
				if (i < 2 * segment - 1) {
					if (j == 0) {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back((i + 1) * (segment + 1) + j + 1);
					}
					else if (j == segment - 1) {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back((i + 1) * (segment + 1) + j);
					}
					else {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back((i + 1) * (segment + 1) + j + 1);

						indices.push_back(i * (segment + 1) + j);
						indices.push_back((i + 1) * (segment + 1) + j + 1);
						indices.push_back((i + 1) * (segment + 1) + j);
					}
				}
				else {
					if (j == 0) {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back(0 * (segment + 1) + j + 1);
					}
					else if (j == segment - 1) {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back(0 * (segment + 1) + j);
					}
					else {
						indices.push_back(i * (segment + 1) + j);
						indices.push_back(i * (segment + 1) + j + 1);
						indices.push_back(0 * (segment + 1) + j + 1);

						indices.push_back(i * (segment + 1) + j);
						indices.push_back(0 * (segment + 1) + j + 1);
						indices.push_back(0 * (segment + 1) + j);
					}
				}
				
			}
		}

		va = new Edge3D::VertexArray();
		vb->subData(0,sizeof(Vec3) * vertices.size(),(float*)vertices.data());
		ib->subData(0,sizeof(unsigned int) * indices.size(),indices.data());
		va->addVertexAttribute(vb,3,GL_FLOAT, false);
		va->addVertexAttribute(vb,3, GL_FLOAT, false);

		srand(time(NULL));
		PointLight light;

		for (int i = 0; i < 5; i++) {
			light.ambient = { 0.3f,0.3f,0.3f };
			light.diffuse = { 0.5f,0.5f,0.5f };
			light.specular = { 0.7f,0.7f,0.7f };
			light.pos = { (float)(rand() % 20 - 10),(float)(rand() % 20 - 10),(float)(rand() % 20 - 10) };
			light.intensity = (float)(rand() % 5);

			lights.push_back(light);
		}


		dLight.ambient = { 0.1f,0.1f,0.1f };
		dLight.diffuse = { 0.4f,0.4f,0.4f };
		dLight.specular = { 0.7f,0.7f,0.7f };
		dLight.direction = { 1,-1,0 };
		
		sLight.pos = { 5,0,0 };
		sLight.ambient = { 0.1f,0.1f,0.1f };
		sLight.diffuse = { 0.4f,0.4f,0.4f };
		sLight.specular = { 0.7f,0.7f,0.7f };
		sLight.direction = { -1,0,0 };
		sLight.innerCut = PI / 6.0f;
		sLight.outerCut = PI / 5.0f;

		mat.ambient = { 0.0f,0.0f,0.3f };
		mat.diffuse = { 0.0f,0.0f,0.5f };
		mat.specular = { 0.7f,0.7f,0.7f };
		mat.shininess = 50;

		positions.push_back({ 0,0,0 });
		positions.push_back({ -1,0,0 });
		positions.push_back({ 1,0,0 });
		positions.push_back({ 0,0,-1 });
		positions.push_back({ 0,0,1 });
		positions.push_back({ 1,0,1 });
		positions.push_back({ -1,0,1 });
		positions.push_back({ 1,0,-1 });
		positions.push_back({ -1,0,-1 });
	}

	~LightDemo() {
		vertices.clear();
		indices.clear();

		delete vb;
		delete va;
		delete ib;
		delete shader;
		
		delete skybox;
	}

	void onCreate() {
		APP_LOG_INFO("Scene onCreate function called, id: {0}", id);
	}

	void onUpdate(float delta) {
		input(delta);
		camera.update(delta);

		skybox->draw(camera);

		shader->bind();
		va->bind();
		ib->bind();

		check();

		shader->setUniform3f("viewerPos",camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		shader->setUniform3f("material.ambient",mat.ambient.x, mat.ambient.y, mat.ambient.z);
		shader->setUniform3f("material.diffuse", mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);
		shader->setUniform3f("material.specular", mat.specular.x, mat.specular.y, mat.specular.z);
		shader->setUniform1f("material.shininess",mat.shininess);

		check();

		for (int i = 0; i < lights.size(); i++) {
			std::string name = "pLight[" + std::to_string(i) + "]";
			shader->setUniform3f((name + std::string(".ambient")).c_str(), lights[i].ambient.x, lights[i].ambient.y, lights[i].ambient.z);
			shader->setUniform3f((name + ".diffuse").c_str(), lights[i].diffuse.x, lights[i].diffuse.y, lights[i].diffuse.z);
			shader->setUniform3f((name + ".specular").c_str(), lights[i].specular.x, lights[i].specular.y, lights[i].specular.z);
			shader->setUniform3f((name + ".pos").c_str(), lights[i].pos.x, lights[i].pos.y, lights[i].pos.z);
			shader->setUniform1f((name + ".constant").c_str(), lights[i].constant);
			shader->setUniform1f((name + ".linear").c_str(), lights[i].linear);
			shader->setUniform1f((name + ".quadrantic").c_str(), lights[i].quadrantic);
			shader->setUniform1f((name + ".intensity").c_str(), lights[i].intensity);
			shader->setUniform1i((name + ".use").c_str(), (int)true);
		}
		
		std::string name = "pLight[" + std::to_string(lights.size()) + "]";
		shader->setUniform1i((name + ".use").c_str(), (int)false);
		
		//SpotLight
		shader->setUniform3f("sLight[0].pos", sLight.pos.x, sLight.pos.y, sLight.pos.z);
		shader->setUniform3f("sLight[0].ambient", sLight.ambient.x, sLight.ambient.y, sLight.ambient.z);
		shader->setUniform3f("sLight[0].diffuse", sLight.diffuse.x, sLight.diffuse.y, sLight.diffuse.z);
		shader->setUniform3f("sLight[0].specular", sLight.specular.x, sLight.specular.y, sLight.specular.z);
		shader->setUniform3f("sLight[0].direction",sLight.direction.x, sLight.direction.y, sLight.direction.z);
		shader->setUniform1f("sLight[0].intensity",sLight.intensity);
		shader->setUniform1f("sLight[0].innerCut",sLight.innerCut);
		shader->setUniform1f("sLight[0].outerCut", sLight.outerCut);
		shader->setUniform1f("sLight[0].constant", sLight.constant);
		shader->setUniform1f("sLight[0].linear", sLight.linear);
		shader->setUniform1f("sLight[0].quadrantic", sLight.quadrantic);
		shader->setUniform1i("sLight[0].use", (int)true);
		shader->setUniform1i("sLight[1].use", (int)false);

		//DirectionalLight
		shader->setUniform3f("dLight.ambient", dLight.ambient.x, dLight.ambient.y, dLight.ambient.z);
		shader->setUniform3f("dLight.diffuse", dLight.diffuse.x, dLight.diffuse.y, dLight.diffuse.z);
		shader->setUniform3f("dLight.specular", dLight.specular.x, dLight.specular.y, dLight.specular.z);
		shader->setUniform3f("dLight.direction", dLight.direction.x, dLight.direction.y, dLight.direction.z);
		shader->setUniform1f("dLight.intensity", dLight.intensity);
		shader->setUniform1i("dLight.use", (int)true);


		
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());


		for (unsigned int i = 0; i < positions.size(); i++) {
			model = Math::translate(positions[i]);
			shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
			 	
	}

	void input(float delta) {

		ImGui::Begin("SpotLight");
		ImGui::SliderFloat3("positions", (float*)&sLight.pos, -10, 10);
		ImGui::SliderFloat3("ambient", (float*)&sLight.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&sLight.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&sLight.specular, 0, 1);
		ImGui::SliderFloat3("direction", (float*)&sLight.direction, -1, 1);
		ImGui::SliderFloat("Intensity", &sLight.intensity, 0, 5);
		ImGui::SliderFloat("InnerCut", &sLight.innerCut, 0, PI / 2.0f);
		ImGui::SliderFloat("OuterCut", &sLight.outerCut, 0, PI / 2.0f);
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

		if (handler.getKeyState(GLFW_KEY_T)) {
			if (lights.size() < 50) {
				PointLight light;

				light.ambient = { 0.3f,0.3f,0.3f };
				light.diffuse = { 0.5f,0.5f,0.5f };
				light.specular = { 0.7f,0.7f,0.7f };
				light.pos = { (float)(rand() % 20 - 10),(float)(rand() % 20 - 10),(float)(rand() % 20 - 10) };
				light.intensity = (float)(rand() % 5);

				lights.push_back(light);
				
			}
		}
		else if (handler.getKeyState(GLFW_KEY_G)) {
			if (lights.size() > 0) {
				lights.pop_back();
			}
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
	
	void check() {
		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL Error: error code = {0}", error);
		}
	}
};