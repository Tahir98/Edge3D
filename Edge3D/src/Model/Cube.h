#pragma once

#include "Shape.h"
#include "OpenGL/Texture.h"

namespace Edge3D {
	class Cube : public Shape {
	public:
		bool drawTexture = 1;

	private:
		Vec3 size = { 1,1,1 }, sTemp = { 1,1,1 };
		Texture* texture = nullptr;

		const char* texturePath = nullptr;

		int m_objectCount = 0;
	public:
		Cube();
		Cube(const char* texturePath);
		Cube(const Vec3 pos, const Vec3 size, const char* texturePath);
		Cube(const Vec3 pos, const Vec3 scale, const Vec3 rotation, const char* texturePath);

		Cube(const Vec3 size, const char* texturePath);
		~Cube();

		void init() override;

		void draw(Camera& camera) override;
		void draw(Camera& camera, Light light) override;
		void imGuiDraw() override;

		void setSize(const Vec3 size);
		Vec3 getSize();

		void setMaterial(Material material);
	};
}

