#pragma once
#include "Shape.h"

namespace Edge3D {
	class Capsule : public Shape {
	public:
	private:
		unsigned int segment = 20, sTemp = 20;

		float radius = 0.5f, rTemp = 0.5f;
		float height = 1, hTemp = 1;

		int m_objectCount = 0;
	public:
		Capsule();
		Capsule(const Vec3 pos, const float radius, const float height);
		Capsule(const Vec3 pos, const Vec3 scale, const Vec3 rotation);
		Capsule(const float radius, const float height, const unsigned int segment);

		~Capsule();

		void init() override;

		void draw(Camera& camera) override;
		void draw(Camera& camera, Light light) override;
		void imGuiDraw() override;

		void setSegment(const unsigned int segment);
		unsigned int getSegment();

		void setRadius(const float radius);
		float getRadius();

		void setHeight(const float height);
		float getHeight();

		void setSize(const float radius, const float height);

		void setMaterial(Material material);
	};
}


