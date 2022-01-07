#pragma once

#include <vector>
#include "Math/Math.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"
#include "Utility/Camera.h"
#include "Utility/Utility.h"

namespace Edge3D {
	class Shape {
	public:
		enum DrawMode {
			POINT = 0, LINE = 1, TRIANGLE = 2
		};

		bool debugMode = false;
		Material material;

	protected:
		VertexArray va;
		VertexBuffer* vb = nullptr;
		IndexBuffer* ib = nullptr;
		IndexBuffer* ibl = nullptr;

		Shader* shader = nullptr;

		std::vector<Vec3> vertices;
		std::vector<unsigned int> indices;
		std::vector<unsigned int> lineIndices;

		Matrix4x4 model;
		Vec3 pos = { 0,0,0 };
		Vec3 scale = { 1,1,1 };
		Vec3 rotation = { 0,0,0 };

		DrawMode mode = DrawMode::TRIANGLE;
		bool point = false, line = false, triangle = true;

	public:
		virtual void init() = 0;

		virtual void draw(Camera& camera) = 0;
		virtual void draw(Camera& camera, Light light) = 0;
		void setDrawMode(DrawMode mode) { this->mode = mode; }
		virtual void imGuiDraw() = 0;

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


