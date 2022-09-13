#pragma once

#include "Edge3D.h"
#include <unordered_map>

class SoftBody : public Edge3D::Scene {
private:
	Edge3D::Window& window;
	Edge3D::SpherecalSkybox skybox;
	Edge3D::PerspectiveCamera camera;
	Edge3D::InputHandler handler;

	Edge3D::FrameBuffer* fb = nullptr;

	bool vsync = true;

	Vec2 now, prev;
	bool isClicked;
	float sensivity = 0.05;
	float cameraSpeed = 10;

	DirectionalLight light;
	Material planeMat;
	Material mat;

	std::vector<Vec3> vertices;
	std::vector<uint32_t> indices;
	std::vector<uint32_t> wire_indices;

	float length = 10;
	float radius = 0.5f;
	uint32_t segment = 12;
	float distance = 1;

	Edge3D::VertexArray va;
	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::Shader* shader = nullptr;

	Edge3D::IndexBuffer* wire_ib = nullptr;
	Edge3D::Shader* wire_shader = nullptr;
	bool wireframe = false;

	float plane_scale = { 10};
	Edge3D::VertexArray plane_va;
	Edge3D::VertexBuffer* plane_vb = nullptr;
	Edge3D::IndexBuffer* plane_ib = nullptr;
	Edge3D::Shader* plane_shader = nullptr;

	struct VertexInfo {
		std::vector<uint32_t> mainIndices;
		std::vector<uint32_t> triIndices;
	};

	//template <>
	//struct hash<Vec3>
	//{
	//	std::size_t operator()(const Vec3& vec3) const
	//	{
	//		using std::size_t;
	//		using std::hash;
	//		using std::string;
	//
	//		// Compute individual hash values for first,
	//		// second and third and combine them using XOR
	//		// and bit shifting:
	//
	//		return ((hash<Vec3>()(vec3.x)
	//			^ (hash<Vec3>()(vec3.y) << 1)) >> 1)
	//			^ (hash<Vec3>()(vec3.z) << 1);
	//	}
	//};


	std::unordered_map<Vec3, VertexInfo> vertexTable;

public:
	SoftBody(Edge3D::Window& window, std::string name) : window(window), Scene(name), skybox("assets/rooitou_park.jpg"), camera(70,9.0f/16.0f,0.1f,500.0f) {

		fb = new Edge3D::FrameBuffer(600, 600);

		light.direction = { 0.2f,-0.8f,0 };
		light.ambient = { 0.3f, 0.3f, 0.3f };
		light.diffuse = { 0.5f, 0.5f, 0.5f };
		light.specular = { 0.7f, 0.7f, 0.7f };
		light.intensity = 1.0f;

		//Plane mesh
		Vec3 plane_vertices[12] = {
			{-1,0,-1}, {0,1,0}, //0
			{-1,0,1},  {0,1,0}, //1
			{1,0,1},   {0,1,0}, //2

			{-1,0,-1}, {0,1,0}, //0
			{1,0,1},   {0,1,0}, //2
			{1,0,-1},  {0,1,0}  //3
		};

		unsigned int plane_indices[] = {
			0,1,2, 0,2,3
		};

		planeMat.ambient = { 0.3f, 0.3f, 0.3f };
		planeMat.diffuse = { 0.5f, 0.5f, 0.5f };
		planeMat.specular = { 0.7f, 0.7f, 0.7f };
		planeMat.shininess = 120;

		plane_vb = new Edge3D::VertexBuffer((float*)plane_vertices,sizeof(Vec3) * 12,GL_STATIC_DRAW);
		plane_ib = new Edge3D::IndexBuffer(plane_indices,sizeof(unsigned int) * 6,GL_STATIC_DRAW);
		plane_shader = new Edge3D::Shader("Shaders/softbody/plane.shader");

		plane_va.addVertexAttribute(plane_vb, 3, GL_FLOAT, false);
		plane_va.addVertexAttribute(plane_vb, 3, GL_FLOAT, false);


		//Soft body object mesh
		float startPoint = -length / 2.0f;
		for (int i = 0; i < length / distance; i++) {
			for (int j = 0; j < segment; j++) {
				float radian = 2.0f * PI * j / (float)segment;
				Vec3 normal = { cosf(radian) ,sinf(radian),0};
				Vec3 position = { normal.x * radius,normal.y * radius + radius + 0.1f, startPoint + distance * i };

				vertices.push_back(position);
				vertices.push_back(normal);
			}
		}

		for (int i = 0; i < length / distance - 1; i++) {
			for (int j = 0; j < segment; j++) {
				indices.push_back(i * segment + j);
				indices.push_back((i + 1) * segment + (j + 1) % segment);
				indices.push_back((i + 1) * segment + j);

				indices.push_back(i * segment + j);
				indices.push_back(i  * segment + (j + 1) % segment);
				indices.push_back((i + 1) * segment + (j + 1) % segment);
			}
		}

		for (int i = 0; i < length / distance - 1; i++) {
			for (int j = 0; j < segment; j++) {
				wire_indices.push_back(i * segment + j);
				wire_indices.push_back(i * segment + (j + 1) % segment);

				wire_indices.push_back(i * segment + j);
				wire_indices.push_back((i + 1) * segment + j);
				
				wire_indices.push_back(i * segment + j);
				wire_indices.push_back((i + 1) * segment + (j + 1) % segment);

				wire_indices.push_back((i + 1) * segment + j);
				wire_indices.push_back((i + 1) * segment + (j + 1) % segment);
			}
		}

		mat.ambient = { 0.3f, 0.0f, 0.0f };
		mat.diffuse = { 0.5f, 0.0f, 0.0f };
		mat.specular = { 0.7f, 0.7f, 0.7f };
		mat.shininess = 120;

		vb = new Edge3D::VertexBuffer((float*)vertices.data(),sizeof(Vec3) * vertices.size(), GL_DYNAMIC_DRAW);
		ib = new Edge3D::IndexBuffer(indices.data(), sizeof(unsigned int) * indices.size(), GL_STATIC_DRAW);
		shader = new Edge3D::Shader("Shaders/softbody/object.shader");
		
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		wire_ib = new Edge3D::IndexBuffer(wire_indices.data(), sizeof(unsigned int) * wire_indices.size(), GL_STATIC_DRAW);
		wire_shader = new Edge3D::Shader("Shaders/softbody/wireframe.shader");

		generateVertexTable();
	}
	
	~SoftBody() {
		delete fb;

		delete vb;
		delete ib;
		delete shader;

		delete plane_vb;
		delete plane_ib;
		delete plane_shader;
	}

	void onCreate() {
		APP_LOG_INFO("Scene onCreate function called, id: {0}", id);
	}

	void onUpdate(float delta) {
		glfwSwapInterval(vsync ? 1 : 0);

		inputControl(delta);
		drawSettingPanel(delta);

		calculateNormals();
		vb->subData(0, sizeof(Vec3) * vertices.size(), (float*)vertices.data());

		camera.update(delta);

		fb->bind();
		fb->clear();
		fb->clearColor(1,1,1,1);
	
		skybox.draw(camera);

		drawPlane();
		
		checkOpenGLErrors();

		drawSoftBodyObject();

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

	void drawPlane() {
		plane_va.bind();
		plane_vb->bind();
		plane_shader->bind();

		plane_shader->setUniform3f("scale", plane_scale, plane_scale, plane_scale);
		plane_shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		plane_shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		plane_shader->setUniform3f("camPos", camera.getPosition());

		plane_shader->setUniform3f("light.direction", light.direction);
		plane_shader->setUniform3f("light.ambient", light.ambient);
		plane_shader->setUniform3f("light.diffuse", light.diffuse);
		plane_shader->setUniform3f("light.specular", light.specular);
		plane_shader->setUniform1f("light.intensity", light.intensity);

		plane_shader->setUniform3f("material.ambient", planeMat.ambient);
		plane_shader->setUniform3f("material.diffuse", planeMat.diffuse);
		plane_shader->setUniform3f("material.specular", planeMat.specular);
		plane_shader->setUniform1f("material.shininess", planeMat.shininess);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void drawSoftBodyObject() {
		
		va.bind();
		vb->bind();
		ib->bind();
		shader->bind();

		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		shader->setUniform3f("camPos", camera.getPosition());

		shader->setUniform3f("light.direction", light.direction);
		shader->setUniform3f("light.ambient", light.ambient);
		shader->setUniform3f("light.diffuse", light.diffuse);
		shader->setUniform3f("light.specular", light.specular);
		shader->setUniform1f("light.intensity", light.intensity);

		shader->setUniform3f("material.ambient", mat.ambient);
		shader->setUniform3f("material.diffuse", mat.diffuse);
		shader->setUniform3f("material.specular", mat.specular);
		shader->setUniform1f("material.shininess", mat.shininess);
	
		glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, 0);

		if (wireframe) {
			glLineWidth(1.2f);

			va.bind();
			wire_shader->bind();
			wire_ib->bind();

			wire_shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
			wire_shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

			glDrawElements(GL_LINES, wire_indices.size(), GL_UNSIGNED_INT, 0);

			glPointSize(4.0f);
			glDrawArrays(GL_POINTS,0,vertices.size());
		}
	}

	void inputControl(float delta) {
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

	void drawSettingPanel(float delta) {
		//Fps and frame time
		ImGui::Begin("Statistics");
		ImGui::Checkbox("Vsync",&vsync);
		ImGui::Checkbox("Wireframe",&wireframe);
		ImGui::Text("FPS: %.2f   frame time: %.2fms",1.0f / delta,delta * 1000.0f);
		ImGui::End();

		//Plane Settings
		ImGui::Begin("Plane");
		ImGui::Text("material");
		ImGui::SliderFloat3("ambient", (float*)&planeMat.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&planeMat.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&planeMat.specular, 0, 1);
		ImGui::SliderFloat("shininess", (float*)&planeMat.shininess, 0, 200);

		ImGui::Separator();
		ImGui::NewLine();
		ImGui::SliderFloat("scale", &plane_scale, 1, 50);
		ImGui::End();

		//Light Settings
		ImGui::Begin("Directional Light");
		ImGui::SliderFloat3("direction", (float*)&light.direction, -1, 1);
		ImGui::SliderFloat3("ambient", (float*)&light.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&light.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&light.specular, 0, 1);
		ImGui::SliderFloat("intensity", (float*)&light.intensity, 0, 5);
		ImGui::End();
		
		//Soft Body Settings
		ImGui::Begin("Soft Body Settings");
		ImGui::SliderFloat3("ambient", (float*)&mat.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&mat.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&mat.specular, 0, 1);
		ImGui::SliderFloat("shininess", (float*)&mat.shininess, 0, 200);
		ImGui::End();
	}

	void generateVertexTable() {
		vertexTable.clear();

		for (int i = 0; i < indices.size(); i++) {
			if (vertexTable.find(vertices[indices[i] * 2]) == vertexTable.end()) {
				vertexTable.insert(std::make_pair(vertices[indices[i] * 2], VertexInfo()));
			}

			vertexTable[vertices[indices[i]] * 2].mainIndices.push_back(i);
			vertexTable[vertices[indices[i]] * 2].triIndices.push_back(i / 3);
		}
	}

	void calculateNormals() {
		for (int i = 0; i < vertices.size()  / 2; i++) {
			VertexInfo& info = vertexTable[vertices[i * 2]];

			Vec3 normal = { 0,0,0 };

			for (int j = 0; j < info.triIndices.size(); j++) {
				Vec3 d1 = vertices[indices[info.triIndices[j] + 1]] - vertices[indices[info.triIndices[j] + 0]];
				Vec3 d2 = vertices[indices[info.triIndices[j] + 1]] - vertices[indices[info.triIndices[j] + 2]];

				normal = normal + Math::crossProduct(Math::normalize(d1), Math::normalize(d2));
			}

			vertices[i * 2 + 1] = Math::normalize(normal);
		}
	}

	void checkOpenGLErrors() {
		int error = 0;

		while ((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL Error, error code:{0}", error);
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