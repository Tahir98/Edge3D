#pragma once
#include "Log.h"
#include "Math/Math.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"

#include "PerspectiveCamera.h"

#include <vector>

namespace Edge3D {
	class LineRenderer3D {
	private:
		Vec3 color;
		float lineWidth = 1.2f;

		uint32_t VB_SIZE = 50000 * 2;

		VertexArray va;
		VertexBuffer* vb = nullptr;
		IndexBuffer* ib = nullptr;
		Shader* shader = nullptr;

		std::vector<Vec3> vertices;
		std::vector<uint32_t> indices;

		bool isStarted = false;

	public:
		LineRenderer3D() {
			vb = new VertexBuffer(nullptr, sizeof(Vec3) * VB_SIZE, GL_DYNAMIC_DRAW);
			ib = new IndexBuffer(nullptr, sizeof(uint32_t) * VB_SIZE * 2, GL_DYNAMIC_DRAW);

			shader = new Shader("Shaders/LineRenderer3D.shader");

			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		}

		LineRenderer3D(Vec3 color) : color(color) {
			vb = new VertexBuffer(nullptr, sizeof(Vec3) * VB_SIZE, GL_DYNAMIC_DRAW);
			ib = new IndexBuffer(nullptr, sizeof(uint32_t) * VB_SIZE * 2, GL_DYNAMIC_DRAW);

			shader = new Shader("Shaders/LineRenderer3D.shader");

			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		}

		LineRenderer3D(uint32_t VB_SIZE) : VB_SIZE(VB_SIZE) {
			vb = new VertexBuffer(nullptr, sizeof(Vec3) * VB_SIZE, GL_DYNAMIC_DRAW);
			ib = new IndexBuffer(nullptr, sizeof(uint32_t) * VB_SIZE * 2, GL_DYNAMIC_DRAW);

			shader = new Shader("Shaders/LineRenderer3D.shader");

			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		}

		~LineRenderer3D() {
			delete vb;
			delete ib;
			delete shader;
		}

		void begin() {
			vertices.clear();
			indices.clear();

		}

		void line(const Vec3 p1, const Vec3 p2) {
			vertices.push_back(p1);
			vertices.push_back(color);

			vertices.push_back(p2);
			vertices.push_back(color);

			if (vertices.size() > VB_SIZE) {
				vertices.clear();
				indices.clear();
			}
		}

		void line(const Vec3 p1, const Vec3 p2, const Vec3 color) {
			vertices.push_back(p1);
			vertices.push_back(color);

			vertices.push_back(p2);
			vertices.push_back(color);

			if (vertices.size() > VB_SIZE) {
				vertices.clear();
				indices.clear();
			}
		}

		void line(const Vec3 p1, const Vec3 color1, const Vec3 p2, const Vec3 color2) {
			vertices.push_back(p1);
			vertices.push_back(color1);

			vertices.push_back(p2);
			vertices.push_back(color2);

			if (vertices.size() > VB_SIZE) {
				vertices.clear();
				indices.clear();
			}
		}

		void end(PerspectiveCamera& camera) {
			ib->subData(0, sizeof(uint32_t) * indices.size(), indices.data());
			vb->subData(0, sizeof(Vec3) * vertices.size(), (float*)vertices.data());

			va.bind();
			ib->bind();

			shader->bind();

			shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
			shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

			glLineWidth(lineWidth);

			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		void setColor(const Vec3 color) {
			this->color = color;
		}

		void setLineWidth(const float lineWidth) {
			this->lineWidth = lineWidth;
		}

		Vec3 getColor() {
			return color;
		}

		float setLineWidth() {
			return lineWidth;
		}
	};
}
