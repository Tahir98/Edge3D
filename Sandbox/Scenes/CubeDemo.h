#pragma once

#include <Edge3D.h>
#include <vector>
class CubeDemo : public Edge3D::Scene {
private:
	struct Cube {
		Vec3 pos;
	};

	Edge3D::Window& window;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::PerspectiveCamera camera;
	Edge3D::InputHandler handler;
	Vec2 prev, now;
	bool isClicked = false;
	float sensivity = 0.025f;
	float cameraSpeed = 30.0f;

	//Cube
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::VertexBuffer* ivb = nullptr;
	Edge3D::VertexArray va;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::Shader shader;
	Edge3D::Texture texture;

	std::vector<Cube> cubes;
	Material material;
	PointLight light;
	unsigned int uniform_buffer;

	int count = 4, tCount = 4;
	float size = 1.0f, tSize = 1.0f;
	float distance = 1.0f, tDistance = 1.0f;

public:
	CubeDemo(Edge3D::Window& window, std::string name) : window(window), Scene(name),skybox("assets/rooitou_park.jpg"), camera(70,9.0f/16.0f,0.1f,1000), shader("Shaders/cube_demo.shader"), texture("assets/au.png") {

		Vec3 vertices[] = {
			{-0.5,-0.5f, 0.5f},  { 0, 0, 1 },   {0,0,0},	//0		// 0
			{-0.5,-0.5f, 0.5f},  {-1, 0, 0 },   {1,0,0},	//1		// 0
			{-0.5,-0.5f, 0.5f},  { 0,-1, 0 },   {0,1,0},	//2		// 0
															//	
			{-0.5, 0.5f, 0.5f},  { 0, 0, 1 },   {0,1,0},	//3		// 1
			{-0.5, 0.5f, 0.5f},  {-1, 0, 0 },   {1,1,0},	//4		// 1
			{-0.5, 0.5f, 0.5f},  { 0, 1, 0 },   {0,0,0},	//5		// 1
															//	
			{ 0.5, 0.5f, 0.5f},  { 0, 0, 1 },   {1,1,0},	//6		// 2
			{ 0.5, 0.5f, 0.5f},  { 1, 0, 0 },   {0,1,0},	//7		// 2
			{ 0.5, 0.5f, 0.5f},  { 0, 1, 0 },   {1,0,0},	//8		// 2
															//	
			{ 0.5,-0.5f, 0.5f},  { 0, 0, 1 },   {1,0,0},	//9		// 3
			{ 0.5,-0.5f, 0.5f},  { 1, 0, 0 },   {0,0,0},	//10	// 3
			{ 0.5,-0.5f, 0.5f},  { 0,-1, 0 },   {1,1,0},	//11	// 3
															//	
			{ 0.5,-0.5f,-0.5f},  { 1, 0, 0 },   {1,0,0},	//12	// 4
			{ 0.5,-0.5f,-0.5f},  { 0, 0,-1 },   {0,0,0},	//13	// 4
			{ 0.5,-0.5f,-0.5f},  { 0,-1, 0 },   {1,0,0},	//14	// 4
															//	
			{ 0.5, 0.5f,-0.5f},  { 1, 0, 0 },   {1,1,0},	//15	// 5
			{ 0.5, 0.5f,-0.5f},  { 0, 0,-1 },   {0,1,0},	//16	// 5
			{ 0.5, 0.5f,-0.5f},  { 0, 1, 0 },   {1,1,0},	//17	// 5
															//	
			{-0.5, 0.5f,-0.5f},  { 0, 0,-1 },   {1,1,0},	//18	// 6
			{-0.5, 0.5f,-0.5f},  {-1, 0, 0 },   {0,1,0},	//19	// 6
			{-0.5, 0.5f,-0.5f},  { 0, 1, 0 },   {0,1,0},	//20	// 6
															//	
			{-0.5,-0.5f,-0.5f},  { 0, 0,-1 },   {1,0,0},	//21	// 7
			{-0.5,-0.5f,-0.5f},  {-1, 0, 0 },   {0,0,0},	//22	// 7
			{-0.5,-0.5f,-0.5f},  { 0,-1, 0 },   {0,0,0}		//23	// 7
		};

		unsigned int indices[36] = {
			0,6,3,		0,9,6,
			10,15,7,	10,12,15,
			13,18,16,	13,21,18,
			22,4,19,	22,1,4,
			23,11,2,	23,14,11,
			5,17,20,	5,8,17
		};

		vb = new Edge3D::VertexBuffer((float*)vertices,sizeof(vertices),GL_STATIC_DRAW);
		ib = new Edge3D::IndexBuffer(indices,sizeof(indices),GL_STATIC_DRAW);

		va.bind();
		vb->bind();
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		material.ambient = { 0.6f,0.6f,0.6f };
		material.diffuse = { 0.5f,0.5f,0.5f };
		material.specular = { 0.7f,0.7f,0.7f };
		material.shininess = 50;

		light.ambient = { 0.6f,0.6f,0.6f };
		light.diffuse = { 0.5f,0.5f,0.5f };
		light.specular = { 0.7f,0.7f,0.7f };
		light.pos = { -5,0,0 };
		light.intensity = 2.0f;

		for (unsigned int i = 0; i < count; i++) {
			for (unsigned int j = 0; j < count; j++) {
				for (unsigned int k = 0; k < count; k++) {
					Vec3 pos;
					float start = -((float)count - 1.0f) / 2.0f * (size + distance);

					pos.x = start + (size + distance) * i;
					pos.y = start + (size + distance) * j;
					pos.z = start + (size + distance) * k;
					
					cubes.push_back({pos});
				}
			}
		}

		glGenBuffers(1, &uniform_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix4x4), nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniform_buffer);

		//shader.bindUniformBuffer("Matrices", 0);

		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4), sizeof(Matrix4x4), camera.getProjectionMatrix());

		ivb = new Edge3D::VertexBuffer(nullptr,3 * sizeof(Vec3) * 8000000,GL_DYNAMIC_DRAW);
	}

	~CubeDemo() {
	
	}

	void onCreate() {
		APP_LOG_INFO("Scene onCreate function called, id: {0}", id);
	}

	void onUpdate(float delta) {
		glEnable(GL_CULL_FACE);
		input(delta);
		if (count != tCount || size != tSize || distance != tDistance) {
			count = tCount;
			size = tSize;
			distance = tDistance;

			cubes.clear();
			cubes.shrink_to_fit();

			for (unsigned int i = 0; i < count; i++) {
				for (unsigned int j = 0; j < count; j++) {
					for (unsigned int k = 0; k < count; k++) {
						Vec3 pos;
						float start = -((float)count - 1.0f) / 2.0f * (size + distance);

						pos.x = start + (size + distance) * i;
						pos.y = start + (size + distance) * j;
						pos.z = start + (size + distance) * k;

						cubes.push_back({ pos });
					}
				}
			}
		}
		

		camera.update(delta);
		skybox.draw(camera);

		glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4x4), camera.getViewMatrix());
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4), sizeof(Matrix4x4), camera.getProjectionMatrix());

		va.bind();
		ib->bind();
		shader.bind();
		texture.bind(0);

		glEnableVertexAttribArray(3);
		ivb->bind();
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		ivb->subData(0,sizeof(Vec3) * cubes.size(), (float*)cubes.data());;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(3, 1);
	
		shader.setUniform3f("light.pos", light.pos.x, light.pos.y, light.pos.z);
		shader.setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		shader.setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader.setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
		shader.setUniform1f("light.intensity", light.intensity);
		shader.setUniform1f("light.constant", light.constant);
		shader.setUniform1f("light.linear", light.linear);
		shader.setUniform1f("light.quadrantic", light.quadrantic);
		shader.setUniform3f("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		shader.setUniform1i("texture1", 0);

		shader.setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
		shader.setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
		shader.setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
		shader.setUniform1f("material.shininess",material.shininess);

		shader.setUniform3f("scale", size, size, size);

		/*for (Cube& cube : cubes) {
			Matrix4x4 model = Math::multiply(Math::translate(cube.pos), true, Math::scale(size, size, size), true);
			//shader.setUniformMatrix4fv("model", 1, true, model.m[0]);
			
		}*/

		glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, cubes.size());
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

	void input(float delta) {
		
		ImGui::Begin("Setting");
		ImGui::Text("Cubes = %d ,fps = %d, Frame time = %.3f",cubes.size() , (int)(1.0f / delta), delta);
		ImGui::SliderInt("Count", &tCount, 1, 100);
		ImGui::SliderFloat("Size", &tSize, 0, 10);
		ImGui::SliderFloat("Distance", &tDistance, 0, 10);

		ImGui::SliderFloat3("Light Pos", (float*)&light.pos, -10, 10);
		ImGui::SliderFloat3("Light Ambient", (float*)&light.ambient, 0, 1);
		ImGui::SliderFloat3("Light Diffuse", (float*)&light.diffuse, 0, 1);
		ImGui::SliderFloat3("Light Specular", (float*)&light.specular, 0, 1);
		ImGui::SliderFloat("Light Intensity", &light.intensity, 0, 3);
		ImGui::Text("\n");

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
	}
};







