#pragma once 
#include "Mesh.h"
#include "Log.h"

namespace Edge3D {
	class SphereMesh : public Mesh {
	private:
		unsigned int verticalSegment = 10;
		unsigned int horizontalSegment = 10;

	public:
		bool includeTexCoords = false;

		void calculate() {
			clear();

			float degreeX, degreeY;
			Vec3 vec3;

			if (includeTexCoords) {
				for (unsigned int i = 0; i < verticalSegment + 1; i++) {
					degreeY = 2.0f * PI * (float)i / (float)verticalSegment;
					for (unsigned int j = 0; j < horizontalSegment + 1; j++) {
						degreeX = PI * (float)j / (float) horizontalSegment;
						vec3.y = cosf(degreeX);
						vec3.x = sinf(degreeY) * sinf(degreeX);
						vec3.z = cosf(degreeY) * sinf(degreeX);

						//std::cout << "x:" << vec3.x << " y:" << vec3.y << " z:" << vec3.z << std::endl;

						vertices.push_back(vec3);
						vertices.push_back(vec3);
						vertices.push_back({ (float)i / (float)verticalSegment, 1.0f - (float)j / (float)horizontalSegment,0 });
					}
				}

				for (unsigned int i = 0; i < verticalSegment; i++) {
					for (unsigned int j = 0; j < horizontalSegment; j++) {
						indices.push_back(i * (horizontalSegment + 1) + j);
						indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
						indices.push_back((i + 1) * (horizontalSegment + 1) + j);

						indices.push_back(i * (horizontalSegment + 1) + j);
						indices.push_back(i * (horizontalSegment + 1) + j + 1);
						indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
					}
				}


				for (unsigned int i = 0; i < verticalSegment; i++) {
					for (unsigned int j = 0; j < horizontalSegment; j++) {
						line_indices.push_back(i * (horizontalSegment + 1) + j);
						line_indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
						line_indices.push_back(i * (horizontalSegment + 1) + j);
						line_indices.push_back((i + 1) * (horizontalSegment + 1) + j);
					
						line_indices.push_back(i * (horizontalSegment + 1) + j);
						line_indices.push_back(i * (horizontalSegment + 1) + j + 1);
					}
				}

			}
			else {
				for (unsigned int i = 0; i < verticalSegment; i++) {
					degreeY = 2.0f * PI * (float)i / (float)verticalSegment;
					for (unsigned int j = 0; j < horizontalSegment + 1; j++) {
						degreeX = PI * (float)j / (float)horizontalSegment;
						vec3.y = cosf(degreeX);
						vec3.x = sinf(degreeY) * sinf(degreeX);
						vec3.z = cosf(degreeY) * sinf(degreeX);

						//std::cout << "x:" << vec3.x << " y:" << vec3.y << " z:" << vec3.z << std::endl;


						vertices.push_back(vec3);
						vertices.push_back(vec3);
					}
				}

				for (unsigned int i = 0; i < verticalSegment; i++) {
					for (unsigned int j = 0; j < horizontalSegment; j++) {
						if (i < verticalSegment - 1) {
							indices.push_back(i * (horizontalSegment + 1) + j);
							indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
							indices.push_back((i + 1) * (horizontalSegment + 1) + j);

							indices.push_back(i * (horizontalSegment + 1) + j);
							indices.push_back(i * (horizontalSegment + 1) + j + 1);
							indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
						}
						else {
							indices.push_back(i * (horizontalSegment + 1) + j);
							indices.push_back((0) * (horizontalSegment + 1) + j + 1);
							indices.push_back((0) * (horizontalSegment + 1) + j);

							indices.push_back(i * (horizontalSegment + 1) + j);
							indices.push_back(i * (horizontalSegment + 1) + j + 1);
							indices.push_back((0) * (horizontalSegment + 1) + j + 1);
						
						}
					}
				}

				for (unsigned int i = 0; i < verticalSegment; i++) {
					for (unsigned int j = 0; j < horizontalSegment; j++) {
						if (i < verticalSegment - 1) {
							line_indices.push_back(i * (horizontalSegment + 1) + j);
							line_indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
							line_indices.push_back((i + 1) * (horizontalSegment + 1) + j);
							
							line_indices.push_back(i * (horizontalSegment + 1) + j);
							line_indices.push_back(i * (horizontalSegment + 1) + j + 1);
							line_indices.push_back((i + 1) * (horizontalSegment + 1) + j + 1);
						}
						else {
							line_indices.push_back(i * (horizontalSegment + 1) + j);
							line_indices.push_back((0) * (horizontalSegment + 1) + j + 1);
							line_indices.push_back((0) * (horizontalSegment + 1) + j);
							
							line_indices.push_back(i * (horizontalSegment + 1) + j);
							line_indices.push_back(i * (horizontalSegment + 1) + j + 1);
							line_indices.push_back((0) * (horizontalSegment + 1) + j + 1);

						}
					}
				}
			}

			vSize = sizeof(Vec3) * vertices.size();
			iSize = sizeof(unsigned int) * indices.size();
			liSize = sizeof(unsigned int) * line_indices.size();
		}

		void setSegment(const unsigned int verticalSegment, const unsigned int horizontalSegment) {
			if (verticalSegment < 3)
				ENG_LOG_ERROR("Sphere Mesh: vertical segment must be at least 3");
			else
				this->verticalSegment = verticalSegment;

			if (horizontalSegment < 2)
				ENG_LOG_ERROR("Sphere Mesh: horizontal segment must be at least 2");
			else
				this->horizontalSegment = horizontalSegment;
		}

		unsigned int getVerticalSegment() {
			return verticalSegment;
		}

		unsigned int getHorizontalSegment() {
			return horizontalSegment;
		}
	};
}

