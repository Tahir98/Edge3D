#pragma once
#include "Shape.h"
#include "OpenGL/Texture.h"
#include "Utility/Utility.h"

namespace Edge3D {
	class Sphere : public Shape {
	public:
		bool drawTexture = 1;

	private:
		unsigned int segment = 20, sTemp = 20;
		float radius = 0.5f, rTemp = 0.5f;
		Texture* texture = nullptr;

		const char* texturePath = nullptr;

		int m_objectCount = 0;
	public:
		Sphere();
		Sphere(const char* texturePath);
		Sphere(const Vec3 pos, const float radius, const char* texturePath);
		Sphere(const Vec3 pos, const Vec3 scale, const Vec3 rotation, const float radius, const char* texturePath);

		Sphere(const float radius, const unsigned int segment, const char* texturePath);
		~Sphere();

		void init() override;

		void draw(Camera& camera) override;
		void draw(Camera& camera, Light light) override;
		void imGuiDraw() override;

		void setSegment(const unsigned int segment);
		unsigned int getSegment();

		void setRadius(const float radius);
		float getRadius();

		void setMaterial(Material material);
	};
}




