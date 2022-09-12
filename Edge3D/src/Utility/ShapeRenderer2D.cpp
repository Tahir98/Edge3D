#pragma once
#include "ShapeRenderer2D.h"
#include <iostream>
#include <math.h>
#include "glad/glad.h"
#include <stdlib.h>

namespace Edge3D {
	float ShapeRenderer2D::toRadian(float degree) {
		return 2.0f * PI * (degree / 360.0f);
	}


	ShapeRenderer2D::ShapeRenderer2D(const float width, const float height) : width(width), height(height), shader("Shaders/shaperenderer2d.shader") {
		va = new VertexArray();
		vb = new VertexBuffer(nullptr, 6 * sizeof(float) * 100000, GL_DYNAMIC_DRAW);
		ib = new IndexBuffer(nullptr, sizeof(unsigned int) * 100000, GL_DYNAMIC_DRAW);

		va->addVertexAttribute(vb, 2, GL_FLOAT, false);
		va->addVertexAttribute(vb, 4, GL_FLOAT, false);
	}

	ShapeRenderer2D::~ShapeRenderer2D() {
		delete va;
		delete vb;
		delete ib;
	}

	void ShapeRenderer2D::begin(ShapeType shapeType) {
		vertices.clear();
		vertices.shrink_to_fit();
		indices.clear();
		indices.shrink_to_fit();

		sBegin = true;
		this->shapeType = shapeType;
	}

	void ShapeRenderer2D::end() {
		if (isBegin()) {
			vb->bind();
			va->bind();

			vb->subData(0, (unsigned int)(vertices.size() * sizeof(float)), vertices.data());

			ib->bind();
			ib->subData(0, (unsigned int)(indices.size() * sizeof(unsigned int)), indices.data());

			shader.bind();
			shader.setUniform1f("width", width);
			shader.setUniform1f("height",height);

			if (shapeType == ShapeType::FILLED) {
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
			}
			else if (shapeType == ShapeType::LINE) {
				glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr);
			}
			sBegin = false;
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
		if (isBegin()) {
			vertices.push_back(x1);
			vertices.push_back(y1);
			addColor();

			vertices.push_back(x2);
			vertices.push_back(y2);
			addColor();

			vertices.push_back(x3);
			vertices.push_back(y3);
			addColor();

			if (shapeType == ShapeType::FILLED) {
				indices.push_back(vertices.size() / 6 - 1 - 2);
				indices.push_back(vertices.size() / 6 - 1 - 1);
				indices.push_back(vertices.size() / 6 - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(vertices.size() / 6 - 1 - 2);
				indices.push_back(vertices.size() / 6 - 1 - 1);

				indices.push_back(vertices.size() / 6 - 1 - 1);
				indices.push_back(vertices.size() / 6 - 1);

				indices.push_back(vertices.size() / 6 - 1);
				indices.push_back(vertices.size() / 6 - 1 - 2);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::triangle(float x1, float y1, float x2, float y2, float x3, float y3, Color color1, Color color2, Color color3) {
		if (isBegin()) {
			vertices.push_back(x1);
			vertices.push_back(y1);
			addColor(color1);

			vertices.push_back(x2);
			vertices.push_back(y2);
			addColor(color2);

			vertices.push_back(x3);
			vertices.push_back(y3);
			addColor(color3);

			if (shapeType == ShapeType::FILLED) {
				indices.push_back(vertices.size() / 6 - 1 - 2);
				indices.push_back(vertices.size() / 6 - 1 - 1);
				indices.push_back(vertices.size() / 6 - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(vertices.size() / 6 - 1 - 2);
				indices.push_back(vertices.size() / 6 - 1 - 1);

				indices.push_back(vertices.size() / 6 - 1 - 1);
				indices.push_back(vertices.size() / 6 - 1);

				indices.push_back(vertices.size() / 6 - 1);
				indices.push_back(vertices.size() / 6 - 1 - 2);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::rect(float x, float y, float width, float height) {
		if (isBegin()) {
			vertices.push_back(x);
			vertices.push_back(y);
			addColor();

			vertices.push_back(x + width);
			vertices.push_back(y);
			addColor();

			vertices.push_back(x + width);
			vertices.push_back(y + height);
			addColor();

			vertices.push_back(x);
			vertices.push_back(y + height);
			addColor();

			unsigned int size = vertices.size() / 6;
			if (shapeType == ShapeType::FILLED) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);

				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);

				indices.push_back(size - 1);
				indices.push_back(size - 1 - 3);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::rect(float x, float y, float width, float height, Color color1, Color color2, Color color3, Color color4) {
		if (isBegin()) {
			vertices.push_back(x);
			vertices.push_back(y);
			addColor(color1);

			vertices.push_back(x + width);
			vertices.push_back(y);
			addColor(color2);

			vertices.push_back(x + width);
			vertices.push_back(y + height);
			addColor(color3);

			vertices.push_back(x);
			vertices.push_back(y + height);
			addColor(color4);

			unsigned int size = vertices.size() / 6;
			if (shapeType == ShapeType::FILLED) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);

				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);

				indices.push_back(size - 1);
				indices.push_back(size - 1 - 3);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::rect(float x, float y, float width, float height, float degree) {
		if (isBegin()) {
			float pos[8];
			pos[0] = x;
			pos[1] = y;

			pos[2] = x = x + width * cosf(toRadian(degree));
			pos[3] = y = y + width * sinf(toRadian(degree));

			degree += 90;
			pos[4] = x = x + height * cosf(toRadian(degree));
			pos[5] = y = y + height * sinf(toRadian(degree));

			degree += 90;
			pos[6] = x = x + width * cosf(toRadian(degree));
			pos[7] = y = y + width * sinf(toRadian(degree));

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			addColor();

			vertices.push_back(pos[2]);
			vertices.push_back(pos[3]);
			addColor();

			vertices.push_back(pos[4]);
			vertices.push_back(pos[5]);
			addColor();

			vertices.push_back(pos[6]);
			vertices.push_back(pos[7]);
			addColor();

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);

				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);

				indices.push_back(size - 1);
				indices.push_back(size - 1 - 3);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::rect(float x, float y, float width, float height, float degree, Color color1, Color color2, Color color3, Color color4) {
		if (isBegin()) {
			float pos[8];
			pos[0] = x;
			pos[1] = y;

			pos[2] = x = x + width * cosf(toRadian(degree));
			pos[3] = y = y + width * sinf(toRadian(degree));

			degree += 90;
			pos[4] = x = x + height * cosf(toRadian(degree));
			pos[5] = y = y + height * sinf(toRadian(degree));

			degree += 90;
			pos[6] = x = x + width * cosf(toRadian(degree));
			pos[7] = y = y + width * sinf(toRadian(degree));

			vertices.push_back(pos[0]);
			vertices.push_back(pos[1]);
			addColor(color1);

			vertices.push_back(pos[2]);
			vertices.push_back(pos[3]);
			addColor(color2);

			vertices.push_back(pos[4]);
			vertices.push_back(pos[5]);
			addColor(color3);

			vertices.push_back(pos[6]);
			vertices.push_back(pos[7]);
			addColor(color4);

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else if (shapeType == ShapeType::LINE) {
				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);

				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 1);

				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);

				indices.push_back(size - 1);
				indices.push_back(size - 1 - 3);
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::circle(float x, float y, float radius) {
		if (isBegin()) {
			float degree = 0;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(degree)));
					vertices.push_back(y + radius * sinf(toRadian(degree)));
					addColor();
					degree += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::circle(float x, float y, float radius, const unsigned int segments) {
		if (isBegin()) {
			float degree = 0;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(degree)));
					vertices.push_back(y + radius * sinf(toRadian(degree)));
					addColor();
					degree += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::circle(float x, float y, float radius, const unsigned int segments, Color center, Color edge) {
		if (isBegin()) {
			float degree = 0;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor(center);
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(degree)));
					vertices.push_back(y + radius * sinf(toRadian(degree)));
					addColor(edge);
					degree += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				std::cout << "This function can only be called FILLED shape type mod" << std::endl;
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}

	}

	void ShapeRenderer2D::ellipse(float x, float y, float a, float b) {
		if (isBegin()) {
			float degree = 0;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + a * cosf(toRadian(degree)));
					vertices.push_back(y + b * sinf(toRadian(degree)));
					addColor();
					degree += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::ellipse(float x, float y, float a, float b, unsigned int segments) {
		if (isBegin()) {
			float degree = 0;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + a * cosf(toRadian(degree)));
					vertices.push_back(y + b * sinf(toRadian(degree)));
					addColor();
					degree += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::ellipse(float x, float y, float a, float b, float degree) {
		if (isBegin()) {
			float deg = degree;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + a * cosf(toRadian(degree)));
					vertices.push_back(y + b * sinf(toRadian(degree)));
					addColor();
					deg += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::ellipse(float x, float y, float a, float b, float degree, unsigned int segments) {
		if (isBegin()) {
			float deg = degree;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + a * cosf(toRadian(degree)));
					vertices.push_back(y + b * sinf(toRadian(degree)));
					addColor();
					deg += 360.0f / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments; i++) {
					if (i == segments - 1) {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+1);
					}
					else {
						indices.push_back(size - 1 - (segments)+i + 1);
						indices.push_back(size - 1 - (segments)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::arc(float x, float y, float radius, float begin, float degree) {

		if (isBegin()) {
			float deg = begin;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(deg)));
					vertices.push_back(y + radius * sinf(toRadian(deg)));
					addColor();
					deg += degree / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					indices.push_back(size - 1 - (segments));
					indices.push_back(size - 1 - (segments)+i);
					indices.push_back(size - 1 - (segments)+i + 1);
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments + 2; i++) {
					if (i == 0) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+1);
					}
					else if (i == segments) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i);
					}
					else {
						indices.push_back(size - 1 - (segments)+i);
						indices.push_back(size - 1 - (segments)+i + 1);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::arc(float x, float y, float radius, float begin, float degree, unsigned int segments) {
		if (isBegin()) {
			float deg = begin;
			for (unsigned int i = 0; i < 2 * segments + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(deg)));
					vertices.push_back(y + radius * sinf(toRadian(deg)));
					addColor();
					deg += degree / segments;
				}
			}

			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < segments; i++) {
					indices.push_back(size - 1 - (segments));
					indices.push_back(size - 1 - (segments)+i);
					indices.push_back(size - 1 - (segments)+i + 1);
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < segments + 2; i++) {
					if (i == 0) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+1);
					}
					else if (i == segments) {
						indices.push_back(size - 1 - (segments));
						indices.push_back(size - 1 - (segments)+i);
					}
					else {
						indices.push_back(size - 1 - (segments)+i);
						indices.push_back(size - 1 - (segments)+i + 1);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::polygon(float x, float y, float radius, const unsigned int edgeCount, float degree) {
		if (edgeCount < 3) {
			std::cout << "Edge count can not be lower than 3" << std::endl;
			return;
		}

		if (isBegin()) {
			float deg = degree;
			for (unsigned int i = 0; i < 2 * edgeCount + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor();
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(deg)));
					vertices.push_back(y + radius * sinf(toRadian(deg)));
					addColor();
					deg += 360.0f / edgeCount;
				}
			}
			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < edgeCount; i++) {
					if (i == edgeCount - 1) {
						indices.push_back(size - 1 - (edgeCount));
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+1);
					}
					else {
						indices.push_back(size - 1 - (edgeCount));
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < edgeCount; i++) {
					if (i == edgeCount - 1) {
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+1);
					}
					else {
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+i + 2);
					}
				}
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}

	}

	void ShapeRenderer2D::polygon(float x, float y, float radius, const unsigned int edgeCount, float degree, Color center, Color edge) {
		if (edgeCount < 3) {
			std::cout << "Edge count can not be lower than 3" << std::endl;
			return;
		}

		if (isBegin()) {

			float deg = degree;
			for (unsigned int i = 0; i < 2 * edgeCount + 2; i += 2) {
				if (i == 0) {
					vertices.push_back(x);
					vertices.push_back(y);
					addColor(center);
				}
				else {
					vertices.push_back(x + radius * cosf(toRadian(deg)));
					vertices.push_back(y + radius * sinf(toRadian(deg)));
					addColor(edge);
					deg += 360.0f / edgeCount;
				}
			}
			unsigned int size = vertices.size() / 6;

			if (shapeType == ShapeType::FILLED) {
				for (unsigned int i = 0; i < edgeCount; i++) {
					if (i == edgeCount - 1) {
						indices.push_back(size - 1 - (edgeCount));
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+1);
					}
					else {
						indices.push_back(size - 1 - (edgeCount));
						indices.push_back(size - 1 - (edgeCount)+i + 1);
						indices.push_back(size - 1 - (edgeCount)+i + 2);
					}
				}
			}
			else if (shapeType == ShapeType::LINE) {
				std::cout << "This function can only be called FILLED shape type mode" << std::endl;
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::rectLine(float x1, float y1, float x2, float y2, float tickness) {
		if (isBegin()) {
			if (shapeType == ShapeType::FILLED) {
				double slope = (y2 - y1) / (x2 - x1);
				float radian = (float)atanf(-1.0f / slope);

				vertices.push_back(x1);
				vertices.push_back(y1);
				addColor();

				vertices.push_back(x1 + tickness * cosf(radian));
				vertices.push_back(y1 + tickness * sinf(radian));
				addColor();

				vertices.push_back(x2);
				vertices.push_back(y2);
				addColor();

				vertices.push_back(x2 + tickness * cosf(radian));
				vertices.push_back(y2 + tickness * sinf(radian));
				addColor();

				unsigned int size = vertices.size() / 6;

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 2);
				indices.push_back(size - 1 - 0);

				indices.push_back(size - 1 - 3);
				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else {
				std::cout << "This function can only be called FILLED shape type mode" << std::endl;
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}
	}

	void ShapeRenderer2D::line(float x1, float y1, float x2, float y2) {
		if (isBegin()) {
			if (shapeType == ShapeType::LINE) {
				vertices.push_back(x1);
				vertices.push_back(y1);
				addColor();

				vertices.push_back(x2);
				vertices.push_back(y2);
				addColor();

				unsigned int size = vertices.size() / 6;

				indices.push_back(size - 1 - 1);
				indices.push_back(size - 1);
			}
			else {
				std::cout << "This function can only be called LINE shape type mode" << std::endl;
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}

	}

	void ShapeRenderer2D::line(float* positions, const unsigned int size) {
		if (isBegin()) {
			if (shapeType == ShapeType::LINE) {
				for (unsigned int i = 0; i < size; i += 2) {
					vertices.push_back(positions[i]);
					vertices.push_back(positions[i + 1]);
					addColor();
				}

				unsigned int ver_size = vertices.size() / 6;
				for (unsigned int i = 0; i < size / 2 - 1; i++) {
					indices.push_back(ver_size - size / 2 + i);
					indices.push_back(ver_size - size / 2 + i + 1);
				}
			}
			else {
				std::cout << "This function can only be called LINE shape type mode" << std::endl;
			}
		}
		else {
			std::cout << "ShapeRenderer must begin first!!!" << std::endl;
		}

	}

	bool ShapeRenderer2D::isBegin() {
		return sBegin;
	}

	void ShapeRenderer2D::setColor(Color color) {
		this->color = color;
	}

	void ShapeRenderer2D::setColor(const float r, const float g, const float b, const float a) {
		color.x = r;
		color.y = g;
		color.z = b;
		color.w = a;
	}

	inline void ShapeRenderer2D::addColor() {
		vertices.push_back(color.x);
		vertices.push_back(color.y);
		vertices.push_back(color.z);
		vertices.push_back(color.w);
	}

	inline void ShapeRenderer2D::addColor(Color c1) {
		vertices.push_back(c1.x);
		vertices.push_back(c1.y);
		vertices.push_back(c1.z);
		vertices.push_back(c1.w);
	}

	float toRadian(float degree) {
		return 2 * PI * (degree / 360.0f);
	}

	void ShapeRenderer2D::setWorldSize(const float width, const float height) {
		if (width > 0)
			this->width = width;

		if (height > 0)
			this->height = height;
	}
}

