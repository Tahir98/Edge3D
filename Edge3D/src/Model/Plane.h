#pragma once

#include "Shape.h"
#include "OpenGL/Texture.h"

namespace Edge3D {
	class Plane : public Shape {
	public:
		bool drawTexture = 1;

	private:
		Vec3 size = { 1,0,1 }, sTemp = { 1,0,1 };
		Texture* texture = nullptr;

		const char* texturePath = nullptr;

		int m_objectCount = 0;
	public:
		Plane();
		Plane(const char* texturePath);
		Plane(const Vec3 pos, const Vec3 size, const char* texturePath);
		Plane(const Vec3 pos, const Vec3 scale, const Vec3 rotation, const char* texturePath);

		Plane(const Vec3 size, const char* texturePath);
		~Plane();

		void init() override;

		void draw(Camera& camera) override;
		void draw(Camera& camera, Light light) override;
		void imGuiDraw() override;

		void setSize(const Vec3 size);
		Vec3 getSize();

		void setMaterial(Material material);
	};
}

