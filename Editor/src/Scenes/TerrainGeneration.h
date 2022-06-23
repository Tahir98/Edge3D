#pragma once
#include "Edge3D.h"
#include <vector>
#include <thread>
#include <chrono>

static Xorshift64 xorshift;
static float lowest = 0, heighest = 0;

class TerrainGeneration : public Edge3D::Scene {
private:
	Edge3D::Window& window;

	struct PerlinNoiseLayer {
		Vec2 offset;
		float scale;
		float height;
		int smoothness;
	};

	struct Vertex {
		Vec3 pos;
		Vec3 normal;
	};

	Edge3D::FrameBuffer* fb = nullptr;

	Edge3D::InputHandler handler;
	Edge3D::SpherecalSkybox skybox;

	Edge3D::PerspectiveCamera camera;
	bool isClicked = false;
	Vec2 now, prev;
	float sensivity = 0.025f;
	float cameraSpeed = 50;

	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::VertexArray va;
	Edge3D::Shader* shader = nullptr;

	Edge3D::IndexBuffer* lib = nullptr;
	Edge3D::Shader* lshader = nullptr;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<uint32_t> lindices;

	uint32_t terrainWidth = 512;
	uint32_t indexCount;
	uint32_t lindexCount;

	Material material;
	DirectionalLight light;

	std::vector<PerlinNoiseLayer> layers;
	PerlinNoiseLayer tempLayer;
	uint32_t selected_layer = 0;
	bool firstTime = true;

	bool wf = false;
	bool vsync = true;

	Vec3 terrainColor = {0,1,0};
	Vec3 rockColor = { 1,1,0 };
	Vec3 snowColor = { 1,1,1 };

public:
	TerrainGeneration(Edge3D::Window& window, std::string name) : window(window), Scene(name), 
	skybox("assets/forest_slope.jpg"), camera(70,9.0f / 16.0f,0.1f,1000) {
		
		fb = new Edge3D::FrameBuffer(960,540);

		PerlinNoiseLayer layer;
		layer.height = 20;
		layer.offset = { 250.323,250.224 };
		layer.scale = 4;
		layer.smoothness = 2;

		layers.push_back(layer);
		tempLayer = layer;

		generateTerrain();
		vb = new Edge3D::VertexBuffer((float*) vertices.data(), sizeof(Vertex) * vertices.size(), GL_STATIC_DRAW);
		ib = new Edge3D::IndexBuffer(indices.data(), sizeof(uint32_t) * indices.size(), GL_STATIC_DRAW);
		lib = new Edge3D::IndexBuffer(lindices.data(), sizeof(uint32_t) * lindices.size(), GL_STATIC_DRAW);

		va.bind();
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		shader = new Edge3D::Shader("Shaders/Terrain2.shader");
		lshader = new Edge3D::Shader("Shaders/twireframe.shader");

		indexCount = indices.size();
		lindexCount = lindices.size();

		material.ambient = Vec3(0.3f, 0.3f, 0.3f);
		material.diffuse = Vec3(0.5f, 0.5f, 0.5f);
		material.specular = Vec3(0.7f, 0.7f, 0.7f);
		material.shininess = 120;

		light.direction = Vec3(0, -1, 0);
		light.ambient = Vec3(0.3f, 0.3f, 0.3f);
		light.diffuse = Vec3(0.5f, 0.5f, 0.5f);
		light.specular = Vec3(0.7f, 0.7f, 0.7f);
		light.intensity = 1.0f;
	}

	~TerrainGeneration() {
		
	}

	void onCreate() {

	}

	void onUpdate(float delta) {
		glfwSwapInterval(vsync ? 1 : 0);

		input(delta);

		fb->bind();
		fb->clear();
		fb->clearColor(1, 1, 1, 1);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		camera.update(delta);
		skybox.draw(camera);

		va.bind();
		ib->bind();
		shader->bind();

		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());	
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());

		shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.x, material.ambient.x);
		shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.x, material.diffuse.x);
		shader->setUniform3f("material.specular", material.specular.x, material.specular.x, material.specular.x);
		shader->setUniform1f("material.shininess", material.shininess);

		shader->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
		shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.x,light.ambient.x);
		shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.x,light.diffuse.x);
		shader->setUniform3f("light.specular", light.specular.x, light.specular.x, light.specular.x);
		shader->setUniform1f("light.intensity", light.intensity);

		shader->setUniform3f("terrainColor", terrainColor.x, terrainColor.y, terrainColor.z);
		shader->setUniform3f("rockColor", rockColor.x, rockColor.y, rockColor.z);
		shader->setUniform3f("snowColor", snowColor.x, snowColor.y, snowColor.z);

		shader->setUniform3f("camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

		if (wf) {
			glLineWidth(1.2f);
			lib->bind();
			lshader->bind();

			lshader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());
			lshader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());

			lshader->setUniform3f("camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

			glDrawElements(GL_LINES, lindexCount, GL_UNSIGNED_INT, 0);
		}

		fb->unbind();

		ImGui::Begin("Terrain");
		ImVec2 PanelSize = ImGui::GetContentRegionAvail();
		Vec2 fbSize = Vec2(fb->getWidth(), fb->getHeight());

		if (PanelSize.x != fbSize.x || PanelSize.y != fbSize.y) {
			fb->resize((uint32_t)PanelSize.x, (uint32_t)PanelSize.y);
			camera.setAspectRatio((float)PanelSize.y / (float)PanelSize.x);
		}
			
		ImGui::Image((ImTextureID)fb->getTextureID(), PanelSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		drawSettingsMenu(delta);
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

	void generateTerrain() {
		Vec2 corner = Vec2(-(float)terrainWidth / 2.0f, -(float)terrainWidth / 2.0f);
		if (firstTime) {
			for (uint32_t i = 0; i < terrainWidth; i++) {
				for (uint32_t j = 0; j < terrainWidth; j++) {
					Vertex vertex;
					vertex.pos = Vec3(corner.x + i, 0, corner.y + j);
					vertex.normal = { 0,1,0 };
					vertices.push_back(vertex);
				}
			}

			for (uint32_t i = 0; i < terrainWidth - 1; i++) {
				for (uint32_t j = 0; j < terrainWidth - 1; j++) {
					indices.push_back(i * terrainWidth + j);
					indices.push_back((i + 1) * terrainWidth + j + 1);
					indices.push_back((i + 1) * terrainWidth + j);

					indices.push_back(i * terrainWidth + j);
					indices.push_back(i * terrainWidth + j + 1);
					indices.push_back((i + 1) * terrainWidth + j + 1);
				}
			}

			for (uint32_t i = 0; i < terrainWidth - 1; i++) {
				for (uint32_t j = 0; j < terrainWidth - 1; j++) {
					lindices.push_back(i * terrainWidth + j);
					lindices.push_back((i + 1) * terrainWidth + j + 1);
					lindices.push_back(i * terrainWidth + j);
					lindices.push_back((i + 1) * terrainWidth + j);
					
					lindices.push_back(i * terrainWidth + j);
					lindices.push_back(i * terrainWidth + j + 1);
				}
			}

			firstTime = false;
		}

		std::thread pThreads[8];
		int start = 0;
		int w = terrainWidth / 8;
		for (int i = 0; i < 8; i++) {
			pThreads[i] = std::thread(calculateNoise, &vertices, &layers, terrainWidth, start, start + w);
			start += w;
		}

		for (int i = 0; i < 8; i++) {
			pThreads[i].join();
		}

		//delete[] pThreads

		for (uint32_t i = 1; i < terrainWidth - 1; i++) {
			for (uint32_t j = 1; j < terrainWidth - 1; j++) {
				Vec3 normal = { 0,0,0 };
				Vec3 pos = vertices[i * terrainWidth + j].pos;

				Vec3 v1 = pos - vertices[(i - 1) * terrainWidth + j - 1].pos;
				Vec3 v2 = vertices[(i - 1) * terrainWidth + j].pos - vertices[(i)*terrainWidth + j - 1].pos;
				normal = normal + Math::crossProduct(v1, v2);

				v1 = vertices[(i)*terrainWidth + j + 1].pos - vertices[(i - 1) * terrainWidth + j].pos;
				v2 = vertices[(i - 1) * terrainWidth + j + 1].pos - pos;
				normal = normal + Math::crossProduct(v1, v2);

				v1 = vertices[(i + 1) * terrainWidth + j + 1].pos - pos;
				v2 = vertices[(i)*terrainWidth + j + 1].pos - vertices[(i + 1) * terrainWidth + j].pos;
				normal = normal + Math::crossProduct(v1, v2);

				v1 = vertices[(i + 1) * terrainWidth + j].pos - vertices[(i)*terrainWidth + j - 1].pos;
				v2 = pos - vertices[(i + 1) * terrainWidth + j - 1].pos;
				normal = normal + Math::crossProduct(v1, v2);

				vertices[i * terrainWidth + j].normal = Math::normalize(normal) * -1.0f;
			}
		}

	}

	static void calculateNoise(std::vector<Vertex>* vertices, std::vector<PerlinNoiseLayer>* layers, uint32_t terrainWidth, int begin, int end) {
		for (uint32_t i = begin; i < end; i++) {
			for (uint32_t j = 0; j < terrainWidth; j++) {
				Vertex vertex = (*vertices)[i * terrainWidth + j];
				float height = 0;
				for (int x = 0; x < (*layers).size(); x++) {
					height += PerlinNoise2D((*layers)[x], (*layers)[x].offset.x + vertex.pos.x * (*layers)[x].scale / 512.0f,
						(*layers)[x].offset.y + vertex.pos.z * (*layers)[x].scale / 512.0f) * (*layers)[x].height;
				};

				(*vertices)[i * terrainWidth + j].pos.y = height;
			}
		}
	}
	

	static float interpolate(float a0, float a1, float w, PerlinNoiseLayer layer) {
		if (w < 0) w = 0;
		else if (w > 1.0f) w = 1.0f;

		switch (layer.smoothness) {
		case 0:
			return (a1 - a0) * w + a0;
			break;
		case 1:
			return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
			break;
		case 2:
			return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
			break;
		default:
			return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
			break;
		}

		return 0;
	}

	static float dotGridGradient(PerlinNoiseLayer layer, int ix, int iz, float x, float z) {
		uint64_t seed = (uint32_t)ix;
		seed = seed << 32;
		seed += (uint32_t)iz;
		xorshift.setSeed(seed);
		float random = xorshift.nextFloat();

		Vec2 gradient;
		gradient.x = cosf(random * PI); gradient.y = sinf(random * PI);

		Vec2 distance = { x - ix , z - iz };
		return Math::dotProduct(gradient, distance);
	}

	static float PerlinNoise2D(PerlinNoiseLayer layer, float x, float z) {
		int x0 = (int)x;
		int x1 = x + 1;
		int z0 = (int)z;
		int z1 = z0 + 1;

		float sx = x - (float)x0;
		float sz = z - (float)z0;

		float n0, n1, n2, n3, ix0, ix1, value;

		n0 = dotGridGradient(layer, x0, z0, x, z);
		n1 = dotGridGradient(layer, x1, z0, x, z);
		ix0 = interpolate(n0, n1, sx, layer);

		n2 = dotGridGradient(layer, x0, z1, x, z);
		n3 = dotGridGradient(layer, x1, z1, x, z);
		ix1 = interpolate(n2, n3, sx, layer);

		value = interpolate(ix0, ix1, sz, layer);

		return value;
	}


	void input(float delta) {
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

	void drawSettingsMenu(float delta) {
		ImGui::Begin("Settings");
		ImGui::Checkbox("wireframe",&wf);
		ImGui::Checkbox("Vsync", &vsync);
		ImGui::Text("Fps: %.2f, frametime: %.2fms",1.0f / delta, delta * 1000.0f);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Material properties");
		ImGui::SliderFloat("mat ambient", (float*)&material.ambient.x, 0, 1);
		ImGui::SliderFloat("mat diffuse", (float*)&material.diffuse.x, 0, 1);
		ImGui::SliderFloat("mat specular", (float*)&material.specular.x, 0, 1);
		ImGui::SliderFloat("mat shininess", &material.shininess, 1, 200);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Light properties");
		ImGui::SliderFloat3("light direction", (float*)&light.direction, -1, 1);
		ImGui::SliderFloat("light ambient", (float*)&light.ambient.x, 0, 1);
		ImGui::SliderFloat("light diffuse", (float*)&light.diffuse.x, 0, 1);
		ImGui::SliderFloat("light specular", (float*)&light.specular.x, 0, 1);
		ImGui::SliderFloat("light intensity", &light.intensity, 0, 5);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Terrain color properties");
		ImGui::ColorEdit3("Terrain Color", (float*)&terrainColor);
		ImGui::ColorEdit3("Rock Color", (float*)&rockColor);
		ImGui::ColorEdit3("Snow Color", (float*)&snowColor);

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Text("Perlin Noise properties");

		std::vector<std::string> layerNames;

		for (int i = 0; i < layers.size(); i++) {
			layerNames.push_back(std::string("Layer" ) + std::to_string(i+1));
		}

		const char* l_name = layerNames.data()->c_str();
		const char* current_name = layers.size() > 0 ? layerNames[selected_layer].c_str() : nullptr;

		if (ImGui::BeginCombo("Layers", current_name)) {
			for (unsigned int i = 0; i < layerNames.size(); i++) {
				bool isSelected = (bool)!strcmp(current_name, layerNames[i].c_str());
				if (ImGui::Selectable(layerNames[i].c_str(), isSelected)) {
					current_name = layerNames[i].c_str();
					selected_layer = i;
					tempLayer = layers[selected_layer];
				}
			}

			ImGui::EndCombo();
		}

		ImGui::DragFloat2("offset", (float*)&layers[selected_layer].offset, 0.1f);
		ImGui::SliderFloat("scale", (float*)&layers[selected_layer].scale,2,128);

		if (layers[selected_layer].height > 512.0f / (layers[selected_layer].scale))
			layers[selected_layer].height = 512.0f / (layers[selected_layer].scale);

		ImGui::SliderFloat("height", &layers[selected_layer].height, 0, 512.0f / (layers[selected_layer].scale));

		

		const char* levels[] = { "Level1","Level2","Level3" };
		int chosenLevel = layers[selected_layer].smoothness;
		const char* combo_label = levels[chosenLevel];
		if (ImGui::BeginCombo("Smoothness", combo_label)) {
			for (int n = 0; n < IM_ARRAYSIZE(levels); n++)
			{
				const bool is_selected = (chosenLevel == n);
				if (ImGui::Selectable(levels[n], is_selected)) {
					chosenLevel = n;
					layers[selected_layer].smoothness = n;
				}	

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Add Layer")) {
			PerlinNoiseLayer layer;
			layer.height = 5;
			layer.offset = { 250.323,250.224 };
			layer.scale = 10.0f;
			layer.smoothness = 2;

			layers.push_back(layer);

			selected_layer = layers.size() - 1;
			tempLayer = layers[selected_layer];

			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Layer")) {
			if (layers.size() >= 1) {
				layers.erase(layers.begin() + selected_layer);
				if (layers.size() >= 1) {
					if(selected_layer > 0)
						selected_layer--;
					tempLayer = layers[selected_layer];
				}
			}

			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
		}
		ImGui::End();

		PerlinNoiseLayer layer = layers[selected_layer];
		if (layer.offset.x != tempLayer.offset.x || layer.offset.y != tempLayer.offset.y) {
			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());

			tempLayer = layers[selected_layer];
		}

		if (layer.scale != tempLayer.scale) {
			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());

			tempLayer = layers[selected_layer];
		}

		if (layer.height != tempLayer.height) {
			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());

			tempLayer = layers[selected_layer];
		}

		if (layer.smoothness != tempLayer.smoothness) {
			generateTerrain();
			vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());

			tempLayer = layers[selected_layer];
		}

		//generateTerrain();
		//vb->subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
	}
};