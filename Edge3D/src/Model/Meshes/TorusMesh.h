#pragma once
#include "Mesh.h"
#include "Log.h"

namespace Edge3D {
    class TorusMesh : public Mesh {
    private:
        unsigned int majorSegment = 20;
        unsigned int minorSegment = 20;
        float majorRadius = 0.5f, minorRadius = 0.1f;
    public:
        void calculate() {
            clear();

			//Vertices
			for (unsigned int i = 0; i < majorSegment; i++) {
				float radY = (2.0f * PI / (float)majorSegment) * (float)i;
				Vec3 iPos;
				iPos.x = cosf(radY) * majorRadius;
				iPos.z = sinf(radY) * majorRadius;
				iPos.y = 0;

				for (unsigned int j = 0; j < minorSegment; j++) {
					float radX = (2.0f * PI / (float)minorSegment) * (float)j;
					Vec3 oPos;
					oPos.y = cosf(radX);
					oPos.x = cosf(radY) * sinf(radX);
					oPos.z = sinf(radY) * sinf(radX);

					vertices.push_back({ iPos.x + oPos.x * minorRadius,iPos.y + oPos.y * minorRadius ,iPos.z + oPos.z * minorRadius });
					vertices.push_back(oPos);
				}
			}

			//Triangles indices 
			for (unsigned int i = 0; i < majorSegment; i++) {
				for (unsigned int j = 0; j < minorSegment; j++) {
					if (i < majorSegment - 1) {
						if (j < minorSegment - 1) {
							indices.push_back(i * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + j + 1);
							indices.push_back(i * minorSegment + j + 1);

							indices.push_back(i * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + j + 1);
						}
						else {
							indices.push_back(i * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + 0);
							indices.push_back(i * minorSegment + 0);

							indices.push_back(i * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + j);
							indices.push_back((i + 1) * minorSegment + 0);
						}
					}
					else {
						if (j < minorSegment - 1) {
							indices.push_back(i * minorSegment + j);
							indices.push_back(0 * minorSegment + j + 1);
							indices.push_back(i * minorSegment + j + 1);
												 
							indices.push_back(i * minorSegment + j);
							indices.push_back(0 * minorSegment + j);
							indices.push_back(0 * minorSegment + j + 1);
						}						 
						else {					  
							indices.push_back(i * minorSegment + j);
							indices.push_back(0 * minorSegment + 0);
							indices.push_back(i * minorSegment + 0);
												
							indices.push_back(i * minorSegment + j);
							indices.push_back(0 * minorSegment + j);
							indices.push_back(0 * minorSegment + 0);
						}
					}
				}
			}

            vSize = sizeof(Vec3) * vertices.size();
            iSize = sizeof(unsigned int) * indices.size();
        }

        void setMajorSegment(const unsigned int segment) {
            if (segment < 3)
                ENG_LOG_ERROR("Torus Mesh: Major segment must be at least 3");
            else
                this->majorSegment = segment;
        }

		void setMinorSegment(const unsigned int segment) {
			if (segment < 3)
				ENG_LOG_ERROR("Torus Mesh: Minor segment must be at least 3");
			else
				this->minorSegment = segment;
		}

		unsigned int getMajorSegment() { return majorSegment; }
		unsigned int getMinorSegment() { return minorSegment; }

		void setMajorRadius(const float radius) {
			if (radius <= 0.0f)
				ENG_LOG_ERROR("Torus Mesh: Major radius must be bigger than 0");
			else
				this->majorRadius = radius;
		}

		void setMinorRadius(const float radius) {
			if (radius <= 0.0f)
				ENG_LOG_ERROR("Torus Mesh: Minor radius must be bigger than 0");
			else
				this->minorRadius = radius;
		}

        float getMajorRadius() { return majorRadius; }
		float getMinorRadius() { return minorRadius; }
    };
}

