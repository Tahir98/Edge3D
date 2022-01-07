#include "Cube.h"
#include "imgui.h"
#include <iostream>
#include <string>

namespace Edge3D {
	static int objectCount = 0;

	Cube::Cube() {
		init();
	}

	Cube::Cube(const char* texturePath) : texturePath(texturePath) {
		init();
	}

	Cube::Cube(const Vec3 pos, const Vec3 size, const char* texturePath) : texturePath(texturePath) {
		this->size = size;
		this->pos = pos;
		init();
	}

	Cube::Cube(const Vec3 pos, const Vec3 scale, const Vec3 rotation, const char* texturePath) : texturePath(texturePath) {
		this->pos = pos;
		this->scale = scale;
		this->rotation = rotation;

		init();
	}

	Cube::Cube(const Vec3 size, const char* texturePath) : texturePath(texturePath) {
		this->size = size;

		init();
	}

	Cube::~Cube() {
		delete vb;
		delete ib;
		delete ibl;
		delete shader;

		delete texture;

		objectCount--;
	}

	void Cube::init() {
		m_objectCount = objectCount++;

		material.ambient = { 0.3f,0.3f,0.3f };
		material.diffuse = { 0.7f,0.7f,0.7f };
		material.specular = { 0.9f,0.9f,0.9f };
		material.shininess = 150;

		shader = new Shader("Shaders/sphereWF.shader");

		vb = new VertexBuffer(nullptr, sizeof(Vec3) * 72, GL_STATIC_DRAW);
		ib = new IndexBuffer(nullptr, sizeof(unsigned int) * 36, GL_STATIC_DRAW);
		ibl = new IndexBuffer(nullptr, sizeof(unsigned int) * 40, GL_STATIC_DRAW);

		va.bind();
		vb->bind();

		va.addVertexAttribute(3, GL_FLOAT, false);
		va.addVertexAttribute(3, GL_FLOAT, false);
		va.addVertexAttribute(3, GL_FLOAT, false);

		if (texturePath)
			texture = new Texture(texturePath);

		setSize(size);
	}

	void Cube::draw(Camera& camera) {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

		shader->bind();
		shader->setUniform1i("tex", 0);

		shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		va.bind();

		if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
			ib->bind();

			shader->setUniform1f("chosen", 2);
			if (drawTexture && texturePath != nullptr) {
				texture->bind(0);
				shader->setUniform1i("drawTexture", 1);
			}
			else
				shader->setUniform1i("drawTexture", 0);

			shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
			shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
			shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
			shader->setUniform1f("material.shininess", material.shininess);

			shader->setUniform3f("light.specular", 0, 0, 0);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
			shader->setUniform1f("chosen", 0);
			glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
		}

		if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
			ibl->bind();
			shader->setUniform1f("chosen", 1);
			glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
		}

		if (debugMode) {
			imGuiDraw();
			if (sTemp.x != size.x || sTemp.y != size.y || sTemp.z != size.z)
				setSize(sTemp);
		}
	}

	void Cube::draw(Camera& camera, Light light) {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

		shader->bind();
		shader->setUniform1i("tex", 0);

		shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		va.bind();

		if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
			ib->bind();

			shader->setUniform1f("chosen", 2);
			if (drawTexture && texturePath != nullptr) {
				texture->bind(0);
				shader->setUniform1i("drawTexture", 1);
			}
			else
				shader->setUniform1i("drawTexture", 0);

			shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
			shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
			shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
			shader->setUniform1f("material.shininess", material.shininess);

			shader->setUniform3f("light.pos", light.pos.x, light.pos.y, light.pos.z);
			shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
			shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
			shader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
			shader->setUniform1f("light.constant", light.constant);
			shader->setUniform1f("light.linear", light.linear);
			shader->setUniform1f("light.quadrantic", light.quadrantic);

			Vec3 viewPos = camera.getPosition();
			shader->setUniform3f("viewPos", viewPos.x, viewPos.y, viewPos.z);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
			shader->setUniform1f("chosen", 0);
			glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
		}

		if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
			ibl->bind();
			shader->setUniform1f("chosen", 1);
			glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
		}

		if (debugMode) {
			imGuiDraw();
			if (sTemp.x != size.x || sTemp.y != size.y || sTemp.z != size.z)
				setSize(sTemp);
		}
	}

	void Cube::imGuiDraw() {
		ImGui::Begin(("Cube" + std::to_string(m_objectCount)).c_str());
		ImGui::SliderFloat3("Position", (float*)&pos, -20, 20);
		ImGui::SliderFloat3("Scale", (float*)&scale, 0, 10);
		ImGui::SliderFloat3("Rotation", (float*)&rotation, -PI, PI);
		ImGui::SliderFloat3("amibent", (float*)&material.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&material.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&material.specular, 0, 1);
		ImGui::SliderFloat("shininess", &material.shininess, 1, 200);

		ImGui::SliderFloat3("Size", (float*)&sTemp, 0, 10);
		ImGui::Checkbox("Point", &point);
		ImGui::Checkbox("Line", &line);
		ImGui::Checkbox("Triangle", &triangle);
		ImGui::Checkbox("Draw Texture", &drawTexture);
		ImGui::End();
	}

	void Cube::setSize(const Vec3 size) {
		if (sTemp.x != size.x || sTemp.y != size.y || sTemp.z != size.z)
			sTemp = size;

		this->size = size;

		vertices.clear();
		vertices.resize(0);
		vertices.shrink_to_fit();

		indices.clear();
		indices.resize(0);
		indices.shrink_to_fit();

		lineIndices.clear();
		lineIndices.resize(0);
		lineIndices.shrink_to_fit();

		vertices = {
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

		indices = {
			0,6,3,		0,9,6,
			10,15,7,	10,12,15,
			13,18,16,	13,21,18,
			22,4,19,	22,1,4,
			23,11,2,	23,14,11,
			5,17,20,	5,8,17
		};

		lineIndices = {
			0,3,3,6,6,0,
			10,7,7,15,15,10,
			13,16,16,18,18,13,
			22,19,19,4,4,22,
			23,2,2,11,11,23,
			5,20,20,17,17,5,

			12,21,11,12
		};


		for (int i = 0; i < vertices.size(); i += 3) {
			vertices[i].x *= size.x;
			vertices[i].y *= size.y;
			vertices[i].z *= size.z;
		}

		//std::cout << "Vertices size: " << vertices.size() << std::endl;
		//std::cout << "Indices size: " << indices.size() << std::endl;
		//std::cout << "LineIndices size: " << lineIndices.size() << std::endl;

		vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
		ib->subData(0, indices.size() * sizeof(unsigned int), indices.data());
		ibl->subData(0, lineIndices.size() * sizeof(unsigned int), lineIndices.data());
	}

	Vec3 Cube::getSize() {
		return size;
	}

	void Cube::setMaterial(Material material) {
		this->material = material;
	}

}

