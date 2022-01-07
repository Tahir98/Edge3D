#pragma once

#include "Shape.h"

namespace Edge3D {
	class Circle3D : public Shape {
	public:
	private:
		unsigned int innerSegment = 20, isTemp = 20;
		unsigned int outerSegment = 20, osTemp = 20;

		float innerRadius = 0.4f, irTemp = 0.4f;
		float outerRadius = 0.1f, orTemp = 0.1f;


		int m_objectCount = 0;
	public:
		Circle3D();
		Circle3D(const Vec3 pos, const float innerRadius, const float outerRadius);
		Circle3D(const Vec3 pos, const Vec3 scale, const Vec3 rotation);
		Circle3D(const float innerRadius, const float outerRadius, const unsigned int innerSegment, const int outerSegment);

		~Circle3D();

		void init();

		void draw(Camera& camera) override;
		void draw(Camera& camera, Light light) override;
		void imGuiDraw() override;

		void setInnerSegment(const unsigned int segment);
		unsigned int getInnerSegment();

		void setOuterSegment(const unsigned int segment);
		unsigned int getOuterSegment();

		void setSegment(const unsigned int inner, const unsigned int outer);

		void setInnerRadius(const float radius);
		float getInnerRadius();

		void setOuterRadius(const float radius);
		float getOuterRadius();

		void setRadius(const float inner, const float outer);

		void setMaterial(Material material);
	};
}

