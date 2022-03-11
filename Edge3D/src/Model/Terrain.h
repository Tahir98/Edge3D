 #pragma once

#include <vector>
#include "Math/Math.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"
#include "Utility/Camera.h"
#include "Utility/Utility.h"
#include <string>
#include <stdlib.h>
#include <time.h>

#include "imgui.h"

namespace Edge3D{
	static int objectCount = 0;

	class Terrain {

	public:
		enum class DrawMode {
			POINT = 0, LINE = 1, TRIANGLE = 2
		};

		bool debugMode = false;
		Material material;
	private:
		VertexArray va1;
		VertexArray va2;
		VertexBuffer* vb1 = nullptr;
		VertexBuffer* vb2 = nullptr;
		IndexBuffer* ib = nullptr;

		Shader* shader = nullptr;

		std::vector<Vec3> vertices;
		std::vector<Vec3> triangles;
		std::vector<unsigned int> lines;

		Matrix4x4 model;
		Vec3 pos = { 0,0,0 };
		Vec3 scale = { 1,1,1 };
		Vec3 rotation = { 0,0,0 };

		DrawMode mode = DrawMode::TRIANGLE;
		bool point = false, line = false, triangle = true;

		unsigned int segmentX = 300;
		unsigned int segmentZ = 300;
		float d = 0.1f;

		int m_objectCount = 0;

	public:
		Terrain() {
			init();
			srand(time(nullptr));
		}

		~Terrain() {
			delete shader;
			delete vb1;
			delete vb2;

			delete ib;

			objectCount--;
		}

		void init() {
			m_objectCount = objectCount++;

			material.ambient = { 0.3f,0.3f,0.3f };
			material.diffuse = { 0.7f,0.7f,0.7f };
			material.specular = { 0.9f,0.9f,0.9f };
			material.shininess = 150;


			for (unsigned int i = 0; i < segmentX; i++) {
				for (unsigned int j = 0; j < segmentZ; j++) {
					Vec3 pos;
					pos.x = ((float)i) * d;
					pos.z = -((float)j) * d;
					if (j > 0 && i == 0) {
						pos.y = vertices[j - 1].y + ((float)(100 - rand() % 200)) / 5000.0f;
					}
					else if (j == 0 && i > 0) {
						pos.y = vertices[i * segmentZ - 1].y + ((float)(100 - rand() % 200)) / 5000.0f;
					}
					else if (i > 0 && j > 0) {
						pos.y = (vertices[i * segmentZ + j - 1].y + vertices[(i - 1) * segmentZ + j].y) / 2.0f + ((float)(100 - rand() % 200)) / 5000.0f;
					}
					else {
						pos.y = 0;
					}
					vertices.push_back(pos);
				}
			}

			for (unsigned int i = 0; i < segmentX - 1; i++) {
				for (unsigned int j = 0; j < segmentZ - 1; j++) {
					Vec3 v1, v2, v3, v4;
					Vec3 normal;

					v1 = vertices[i * segmentZ + j];
					v2 = vertices[i * segmentZ + j + 1];
					v3 = vertices[(i + 1) * segmentZ + j];
					v4 = vertices[(i + 1) * segmentZ + j + 1];

					normal = Math::crossProduct(v1 - v2, v3 - v2);

					triangles.push_back(v1);
					triangles.push_back(normal);

					triangles.push_back(v3);
					triangles.push_back(normal);

					triangles.push_back(v2);
					triangles.push_back(normal);

					normal = Math::crossProduct(v2 - v4, v3 - v4);

					triangles.push_back(v2);
					triangles.push_back(normal);


					triangles.push_back(v3);
					triangles.push_back(normal);

					triangles.push_back(v4);
					triangles.push_back(normal);
				}
			}

			for (unsigned int i = 0; i < segmentX - 1; i++) {
				for (unsigned int j = 0; j < segmentZ - 1; j++) {

					lines.push_back(i * segmentX + j);
					lines.push_back(i * segmentX + j + 1);

					lines.push_back(i * segmentX + j + 1);
					lines.push_back((i + 1) * segmentX + j);

					lines.push_back(i * segmentX + j);
					lines.push_back((i + 1) * segmentX + j);

					lines.push_back((i + 1) * segmentX + j);
					lines.push_back((i + 1) * segmentX + j + 1);

					lines.push_back((i + 1) * segmentX + j + 1);
					lines.push_back(i * segmentX + j + 1);
				}
			}

			shader = new Shader("Shaders/terrain.shader");

			vb1 = new VertexBuffer((float*)vertices.data(), sizeof(Vec3) * vertices.size(), GL_STATIC_DRAW);
			vb2 = new VertexBuffer((float*)triangles.data(), sizeof(Vec3) * triangles.size(), GL_STATIC_DRAW);

			ib = new IndexBuffer(lines.data(), sizeof(unsigned int) * lines.size(), GL_STATIC_DRAW);

			vb1->bind();
			va1.bind();

			va1.addVertexAttribute(vb1,3, GL_FLOAT, false);

			vb2->bind();
			va2.bind();

			va2.addVertexAttribute(vb2, 3, GL_FLOAT, false);
			va2.addVertexAttribute(vb2, 3, GL_FLOAT, false);
		}

		void draw(Camera& camera) {
			shader->bind();

			model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

			shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
			shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
			shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

			if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
				va2.bind();
				shader->setUniform1f("chosen", 2);

				shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
				shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
				shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
				shader->setUniform1f("material.shininess", material.shininess);

				shader->setUniform3f("light.specular", 0, 0, 0);

				glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);
			}

			if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
				shader->setUniform1f("chosen", 0);
				va1.bind();

				glDrawArrays(GL_POINTS, 0, vertices.size());
			}

			if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
				va1.bind();
				ib->bind();
				shader->setUniform1f("chosen", 1);
				glDrawElements(GL_LINES, lines.size(), GL_UNSIGNED_INT, 0);
			}

			if (debugMode) {
				imGuiDraw();
			}
		}

		void draw(Camera& camera, PointLight light) {
			shader->bind();

			model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

			shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
			shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
			shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

			if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
				va2.bind();
				shader->setUniform1f("chosen", 2);

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

				glDrawArrays(GL_TRIANGLES, 0, triangles.size() / 2);
			}

			if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
				shader->setUniform1f("chosen", 0);
				va1.bind();

				glDrawArrays(GL_POINTS, 0, vertices.size());
			}

			if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
				va1.bind();
				ib->bind();
				shader->setUniform1f("chosen", 1);
				glDrawElements(GL_LINES, lines.size(), GL_UNSIGNED_INT, 0);
			}

			if (debugMode) {
				imGuiDraw();
			}
		}

		void imGuiDraw() {
			ImGui::Begin(("Terrain" + std::to_string(m_objectCount)).c_str());
			ImGui::SliderFloat3("Position", (float*)&pos, -20, 20);
			ImGui::SliderFloat3("Scale", (float*)&scale, 0, 10);
			ImGui::SliderFloat3("Rotation", (float*)&rotation, -PI, PI);
			ImGui::SliderFloat3("amibent", (float*)&material.ambient, 0, 1);
			ImGui::SliderFloat3("diffuse", (float*)&material.diffuse, 0, 1);
			ImGui::SliderFloat3("specular", (float*)&material.specular, 0, 1);
			ImGui::SliderFloat("shininess", &material.shininess, 1, 200);
			ImGui::Checkbox("Point", &point);
			ImGui::Checkbox("Line", &line);
			ImGui::Checkbox("Triangle", &triangle);
			ImGui::End();
		}

		void setMaterial(Material mat) {
			this->material = mat;
		}

		void setDrawMode(DrawMode mode) {
			this->mode = mode;
		}

		void setPosition(const Vec3 pos) {
			this->pos = pos;
		}
		Vec3 getPosition() {
			return pos;
		}

		void setScale(const Vec3 scale) {
			this->scale = scale;
		}
		Vec3 getScale() {
			return scale;
		}

		void rotate(Vec3 rot) {
			rotation = rotation + rot;
		}
		void setRotation(Vec3 rotation) {
			this->rotation = rotation;
		}
		Vec3 getRotation() {
			return rotation;
		}
	};
}

