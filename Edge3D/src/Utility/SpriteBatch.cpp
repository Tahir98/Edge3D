#include "SpriteBatch.h"
#include "glad/glad.h"
#include <iostream>
#include <string>

namespace Edge3D {
	SpriteBatch::SpriteBatch(const float width,const float height) :width(width), height(height),vb(nullptr, sizeof(float) * 100000, GL_DYNAMIC_DRAW), shader("Shaders/batch.shader") {
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexSlots);
		std::cout << "Max texture units: " << maxTexSlots << std::endl;

		va.addVertexAttribute(&vb, 2, GL_FLOAT, false);
		va.addVertexAttribute(&vb, 2, GL_FLOAT, false);
		va.addVertexAttribute(&vb, 1, GL_FLOAT, false);
	}

	SpriteBatch::~SpriteBatch() {
		vertices.clear();
	}

	void SpriteBatch::begin() {
		vertices.clear();
		ids.clear();
		usedTexSlots = 0;

		isBegin = true;
	}

	void SpriteBatch::end() {
		if (isBegin) {
			isBegin = false;

			shader.bind();
			va.bind();
			vb.bind();
			vb.subData(0, sizeof(float) * vertices.size(), vertices.data());

			shader.setUniform1f("width", width);
			shader.setUniform1f("height",height);


			int tex[32] = { 0 };
			for (unsigned int i = 0; i < ids.size(); i++) {
				tex[i] = ids[i].second;
			}

			shader.setUniformiv("tex", 32, tex);

			glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

			vertices.clear();
			ids.clear();
			usedTexSlots = 0;
		}
		else {
			std::cout << "SpriteBatch must begin first" << std::endl;
		}
	}

	void SpriteBatch::draw(Texture& texture, const float x, const float y, const float width, const float height) {
		if (isBegin) {
			bool isExist = false;
			int slot;
			for (int i = 0; i < ids.size(); i++) {
				if (texture.getId() == ids[i].first) {
					isExist = true;
					slot = ids[i].second;
					break;
				}
			}

			if (isExist) {
				addVertices(x, y, width, height, slot);
			}
			else {
				glActiveTexture(GL_TEXTURE0 + usedTexSlots);
				glBindTexture(GL_TEXTURE_2D, texture.getId());

				ids.push_back(std::make_pair(texture.getId(), usedTexSlots));

				addVertices(x, y, width, height, usedTexSlots);

				usedTexSlots++;

				if (usedTexSlots == maxTexSlots) {
					shader.bind();
					va.bind();
					vb.bind();

					vb.subData(0, sizeof(float) * vertices.size(), vertices.data());

					shader.setUniform1f("width", width);
					shader.setUniform1f("height",height);

					int tex[32] = { 0 };
					for (unsigned int i = 0; i < ids.size(); i++) {
						tex[i] = ids[i].second;
					}

					shader.setUniformiv("tex", 32, tex);

					glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);

					vertices.clear();
					ids.clear();
					usedTexSlots = 0;
				}
			}
		}
		else {
			std::cout << "SpriteBatch must begin first" << std::endl;
		}

	}

	inline void SpriteBatch::addVertices(const float x, const float y, const float width, const float height, const int slot) {
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(0);
		vertices.push_back(0);
		vertices.push_back((float)slot);

		vertices.push_back(x + width);
		vertices.push_back(y);
		vertices.push_back(1);
		vertices.push_back(0);
		vertices.push_back((float)slot);

		vertices.push_back(x + width);
		vertices.push_back(y + height);
		vertices.push_back(1);
		vertices.push_back(1);
		vertices.push_back((float)slot);

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(0);
		vertices.push_back(0);
		vertices.push_back((float)slot);

		vertices.push_back(x + width);
		vertices.push_back(y + height);
		vertices.push_back(1);
		vertices.push_back(1);
		vertices.push_back((float)slot);

		vertices.push_back(x);
		vertices.push_back(y + height);
		vertices.push_back(0);
		vertices.push_back(1);
		vertices.push_back((float)slot);
	}
}


