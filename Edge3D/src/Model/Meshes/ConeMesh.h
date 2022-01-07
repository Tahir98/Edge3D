#pragma once
#include "Mesh.h"
#include "Log.h"

namespace Edge3D {
	class ConeMesh : public Mesh {
	private:
		unsigned int segment = 20;
        float radius = 0.5f, height = 1.0f;
	public:
		void calculate() {
			clear();

            //Vertcies
            for (unsigned int i = 0; i < segment; i++) {
                float radian = (i + 0.5f) * 2.0f * PI / (float)segment;
                Vec3 pos;

                float length = sqrtf(radius * radius + height * height);
                float coneX = radius / length;
                float coneY = -height / length;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ 0, height / 2.0f,0 });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });

                radian = i * 2.0f * PI / (float)segment;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ pos.x * radius, pos.y, pos.z * radius });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });
            }

            vertices.push_back({ 0, -height / 2.0f,0 });
            vertices.push_back({ 0,-1,0 });

            for (unsigned int i = 0; i < segment; i++) {
                float radian = i * 2.0f * PI / (float)segment;
                Vec3 pos;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian) * radius;
                pos.z = sinf(radian) * radius;

                vertices.push_back(pos);
                vertices.push_back({ 0,-1,0 });
            }

            //Indices
            for (unsigned int i = 0; i < segment * 2; i += 2) {
                if (i < 2 * segment - 2) {
                    indices.push_back(i);
                    indices.push_back(i + 3);
                    indices.push_back(i + 1);
                }
                else {
                    indices.push_back(i);
                    indices.push_back(1);
                    indices.push_back(i + 1);
                }
            }

            for (unsigned int i = 2 * segment + 1; i < 3 * segment + 1; i++) {
                if (i < 3 * segment) {
                    indices.push_back(2 * segment);
                    indices.push_back(i);
                    indices.push_back(i + 1);
                }
                else {
                    indices.push_back(2 * segment);
                    indices.push_back(i);
                    indices.push_back(2 * segment + 1);
                }
            }

			vSize = sizeof(Vec3) * vertices.size();
			iSize = sizeof(unsigned int) * indices.size();
		}

		void setSegment(const unsigned int segment) {
			if (segment < 3)
				ENG_LOG_ERROR("Cone Mesh: segment must be at least 3");
			else
				this->segment = segment;
		}

		unsigned int getSegment() { return segment; }

        void setHeight(const float height) {
            if (height <= 0.0f)
                ENG_LOG_ERROR("Cone Mesh: height must be bigger than 0");
            else
                this->height = height;
        }

        float getHeight() { return height; }

        void setRadius(const float radius) {
            if (radius <= 0.0f)
                ENG_LOG_ERROR("Cone Mesh: radius must be bigger than 0");
            else
                this->radius = radius;
        }

        float getRadius() { return radius; }
	};
}

