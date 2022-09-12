#pragma once
#include "Math/Math.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"
#include "Utility/Camera.h"
#include <vector>
#include <iostream>
#include "stb_image.h"
#include "glad/glad.h"

namespace Edge3D {
	class SpherecalSkybox {
	private:
		VertexArray va;
		VertexBuffer* vb = nullptr;
		IndexBuffer* ib = nullptr;
		Shader* shader = nullptr;

		unsigned int size = 0;
		unsigned int textureID = 0;

	public:
		SpherecalSkybox(const char* texturePath) {
			std::vector<Vec3> vertices;
			std::vector<unsigned int> indices;

			float degY = 0;
			float degX = 0;

			float x, y, z;
			int segment = 40;
			float radius = 10;

			for (unsigned int i = 0; i < 2 * segment + 1; i++) {
				degY = (180.0f / (float)segment) * i;

				for (unsigned int j = 0; j < segment + 1; j++) {
					degX = (180.0f / (float)segment) * j;

					y = cosf(Math::toRadian(degX));
					x = sinf(Math::toRadian(degY)) * sinf(Math::toRadian(degX));
					z = cosf(Math::toRadian(degY)) * sinf(Math::toRadian(degX));

					vertices.push_back({ x * radius, y * radius,z * radius });
					vertices.push_back({ 1.0f - ((float)i) / (2.0f * (float)segment),((float)segment - (float)j) / (float)segment,0 });
				}
			}

			for (unsigned int i = 0; i < 2 * segment; i++) {
				for (unsigned int j = 0; j < segment; j++) {
					if (i < 2 * segment) {
						if (j == 0) {
							indices.push_back(i * (segment + 1) + j);
							indices.push_back((i + 1) * (segment + 1) + j + 1);
							indices.push_back(i * (segment + 1) + j + 1);
						}
						else if (j == segment - 1) {
							indices.push_back(i * (segment + 1) + j);
							indices.push_back((i + 1) * (segment + 1) + j);
							indices.push_back(i * (segment + 1) + j + 1);
						}
						else {
							indices.push_back(i * (segment + 1) + j);
							indices.push_back((i + 1) * (segment + 1) + j + 1);
							indices.push_back(i * (segment + 1) + j + 1);

							indices.push_back(i * (segment + 1) + j);
							indices.push_back((i + 1) * (segment + 1) + j);
							indices.push_back((i + 1) * (segment + 1) + j + 1);
						}
					}
				}
			}

			vb = new VertexBuffer((float*)vertices.data(), sizeof(Vec3) * vertices.size(), GL_STATIC_DRAW);
			ib = new IndexBuffer(indices.data(), sizeof(unsigned int) * indices.size(), GL_STATIC_DRAW);

			size = indices.size();

			va.bind();
			vb->bind();

			va.addVertexAttribute(vb, 3, GL_FLOAT, false);
			va.addVertexAttribute(vb, 3, GL_FLOAT, false);

			int width, height, bpp;

			stbi_set_flip_vertically_on_load(true);
			unsigned char* image = stbi_load(texturePath, &width, &height, &bpp, 4);

			if (image == nullptr) {
				ENG_LOG_ERROR("Texture not found: {0}", texturePath);
			}
			else {
				glGenTextures(1, &textureID);

				glBindTexture(GL_TEXTURE_2D, textureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				ENG_LOG_INFO("Texture loaded successfuly, {0}, width: {1}, height: {2}, bpp: {3}", texturePath, width, height, bpp);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

				stbi_image_free(image);
			}

			shader = new Shader("Shaders/skybox.shader");

		}

		~SpherecalSkybox() {
			delete vb;
			delete ib;
			delete shader;

			glDeleteTextures(1, &textureID);
		}


		void draw(Camera& camera) {
			glEnable(GL_DEPTH_TEST);
			//
			shader->bind();
			shader->setUniform1i("tex", 0);

			Matrix4x4 view;
			float* value = camera.getViewMatrix();
			for (unsigned int i = 0; i < 16; i++) {
				view.m[i / 4][i % 4] = value[i];
				if (i >= 12 && i < 15)
					view.m[i / 4][i % 4] = 0;
			}
			
			shader->setUniformMatrix4fv("view", 1, true, view.m[0]);
			shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());
			//
			va.bind();
			ib->bind();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
		}

		uint32_t& getTextureID() {
			return textureID;
		}
	};


}

