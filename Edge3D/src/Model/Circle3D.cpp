#include "Circle3D.h"
#include <iostream>
#include <string>
#include "imgui.h"

namespace Edge3D {
	static int objectCount = 0;

	Circle3D::Circle3D() {
		init();
	}

	Circle3D::Circle3D(const Vec3 pos, const float innerRadius, const float outerRadius) {
		this->pos = pos;
		if (innerRadius >= 0)
			irTemp = this->innerRadius = innerRadius;
		if (outerRadius >= 0)
			orTemp = this->outerRadius = outerRadius;

		init();
	}

	Circle3D::Circle3D(const Vec3 pos, const Vec3 scale, const Vec3 rotation) {
		this->pos = pos;
		this->scale = scale;
		this->rotation = rotation;
	}

	Circle3D::Circle3D(const float innerRadius, const float outerRadius, const unsigned int innerSegment, const int outerSegment) {
		if (innerRadius >= 0)
			irTemp = this->innerRadius = innerRadius;
		if (outerRadius >= 0)
			orTemp = this->outerRadius = outerRadius;

		if (innerSegment >= 3 && innerSegment <= 100)
			isTemp = this->innerSegment = innerSegment;
		if (outerSegment >= 3 && outerSegment <= 50)
			osTemp = this->outerSegment = outerSegment;

		init();
	}

	Circle3D::~Circle3D() {
		delete vb;
		delete ib;
		delete ibl;
		delete shader;

		vertices.clear();
		vertices.resize(0);
		vertices.shrink_to_fit();

		indices.clear();
		indices.resize(0);
		indices.shrink_to_fit();

		lineIndices.clear();
		lineIndices.resize(0);
		lineIndices.shrink_to_fit();

		objectCount--;
	}

	void Circle3D::init() {
		m_objectCount = objectCount++;

		material.ambient = { 0.3f,0.3f,0.3f };
		material.diffuse = { 0.7f,0.7f,0.7f };
		material.specular = { 0.9f,0.9f,0.9f };
		material.shininess = 150;

		shader = new Shader("Shaders/shape.shader");

		vb = new VertexBuffer(nullptr, sizeof(Vec3) * 10000, GL_STATIC_DRAW);

		ib = new IndexBuffer(nullptr, sizeof(unsigned int) * 30000, GL_STATIC_DRAW);
		ibl = new IndexBuffer(nullptr, sizeof(unsigned int) * 30000, GL_STATIC_DRAW);

		va.bind();
		vb->bind();

		va.addVertexAttribute(vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(vb, 3, GL_FLOAT, false);

		setSegment(innerSegment, outerSegment);
	}

	void Circle3D::draw(Camera& camera) {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		//model = Math::translate(pos) * Math::scale(scale) * Math::rotate(rotation);
		model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

		shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		va.bind();
		vb->bind();

		if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
			ib->bind();
			shader->setUniform1f("chosen", 2);

			shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
			shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
			shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
			shader->setUniform1f("material.shininess", material.shininess);

			shader->setUniform3f("light.specular", 0, 0, 0);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
			shader->setUniform1f("chosen", 0);
			glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
		}

		if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
			ibl->bind();
			shader->setUniform1f("chosen", 1);
			glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
		}

		if (debugMode) {
			imGuiDraw();
			if (isTemp != innerSegment || osTemp != outerSegment)
				setSegment(isTemp, osTemp);

			if (irTemp != innerRadius || orTemp != outerRadius)
				setRadius(irTemp, orTemp);
		}
	}

	void Circle3D::draw(Camera& camera, PointLight light) {
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		//model = Math::translate(pos) * Math::scale(scale) * Math::rotate(rotation);
		model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

		shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		va.bind();
		vb->bind();

		if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
			ib->bind();
			shader->setUniform1f("chosen", 2);

			shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
			shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
			shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
			shader->setUniform1f("material.shininess", material.shininess);

			shader->setUniform3f("light.pos", light.pos.x, light.pos.y, light.pos.z);
			shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
			shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
			shader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
			shader->setUniform1f("light.constant", light.constant);
			shader->setUniform1f("light.linear", light.linear);
			shader->setUniform1f("light.quadrantic", light.quadrantic);

			Vec3 viewPos = camera.getPosition();
			shader->setUniform3f("viewPos", viewPos.x, viewPos.y, viewPos.z);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}

		if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
			shader->setUniform1f("chosen", 0);
			glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
		}

		if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
			ibl->bind();
			shader->setUniform1f("chosen", 1);
			glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
		}

		if (debugMode) {
			imGuiDraw();
			if (isTemp != innerSegment || osTemp != outerSegment)
				setSegment(isTemp, osTemp);

			if (irTemp != innerRadius || orTemp != outerRadius)
				setRadius(irTemp, orTemp);
		}
	}

	void Circle3D::imGuiDraw() {
		ImGui::Begin(("Circle3D" + std::to_string(m_objectCount)).c_str());
		ImGui::SliderFloat3("Position", (float*)&pos, -20, 20);
		ImGui::SliderFloat3("Scale", (float*)&scale, 0, 10);
		ImGui::SliderFloat3("Rotation", (float*)&rotation, -PI, PI);
		ImGui::SliderFloat3("amibent", (float*)&material.ambient, 0, 1);
		ImGui::SliderFloat3("diffuse", (float*)&material.diffuse, 0, 1);
		ImGui::SliderFloat3("specular", (float*)&material.specular, 0, 1);
		ImGui::SliderFloat("shininess", &material.shininess, 1, 200);
		ImGui::SliderInt("Inner Segment", (int*)&isTemp, 3, 100);
		ImGui::SliderInt("Outer Segment", (int*)&osTemp, 3, 50);
		ImGui::SliderFloat("Inner Radius", &irTemp, 0, 10);
		ImGui::SliderFloat("Outer Radius", &orTemp, 0, 10);
		ImGui::Checkbox("Point", &point);
		ImGui::Checkbox("Line", &line);
		ImGui::Checkbox("Triangle", &triangle);
		ImGui::End();
	}

	void Circle3D::setInnerSegment(const unsigned int segment) {
		setSegment(segment, outerSegment);
	}

	unsigned int Circle3D::getInnerSegment() {
		return innerSegment;
	}

	void Circle3D::setOuterSegment(const unsigned int segment) {
		setSegment(innerSegment, segment);
	}

	unsigned int Circle3D::getOuterSegment() {
		return outerSegment;
	}

	void Circle3D::setSegment(const unsigned int inner, const unsigned int outer) {
		if ((inner < 3 || inner > 100) && (outer < 3 || outer > 50))
			return;

		if (inner >= 3 && inner <= 100) {
			this->innerSegment = inner;
			if (isTemp != inner)
				isTemp = inner;
		}

		if (outer >= 3 && outer <= 50) {
			this->outerSegment = outer;
			if (osTemp != outer)
				osTemp = outer;
		}

		vertices.clear();
		vertices.resize(0);
		vertices.shrink_to_fit();

		indices.clear();
		indices.resize(0);
		indices.shrink_to_fit();

		lineIndices.clear();
		lineIndices.resize(0);
		lineIndices.shrink_to_fit();

		for (unsigned int i = 0; i < innerSegment; i++) {
			float radY = (2.0f * PI / (float)innerSegment) * (float)i;
			Vec3 iPos;
			iPos.x = cosf(radY) * innerRadius;
			iPos.z = sinf(radY) * innerRadius;
			iPos.y = 0;

			for (unsigned int j = 0; j < outerSegment; j++) {
				float radX = (2.0f * PI / (float)outerSegment) * (float)j;
				Vec3 oPos;
				oPos.y = cosf(radX);
				oPos.x = cosf(radY) * sinf(radX);
				oPos.z = sinf(radY) * sinf(radX);

				vertices.push_back({ iPos.x + oPos.x * outerRadius,iPos.y + oPos.y * outerRadius ,iPos.z + oPos.z * outerRadius });
				vertices.push_back(oPos);
			}
		}

		//Triangles
		for (unsigned int i = 0; i < innerSegment; i++) {
			for (unsigned int j = 0; j < outerSegment; j++) {
				if (i < innerSegment - 1) {
					if (j < outerSegment - 1) {
						indices.push_back(i * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + j + 1);
						indices.push_back(i * outerSegment + j + 1);

						indices.push_back(i * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + j + 1);
					}
					else {
						indices.push_back(i * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + 0);
						indices.push_back(i * outerSegment + 0);

						indices.push_back(i * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + j);
						indices.push_back((i + 1) * outerSegment + 0);
					}
				}
				else {
					if (j < outerSegment - 1) {
						indices.push_back(i * outerSegment + j);
						indices.push_back(0 * outerSegment + j + 1);
						indices.push_back(i * outerSegment + j + 1);

						indices.push_back(i * outerSegment + j);
						indices.push_back(0 * outerSegment + j);
						indices.push_back(0 * outerSegment + j + 1);
					}
					else {
						indices.push_back(i * outerSegment + j);
						indices.push_back(0 * outerSegment + 0);
						indices.push_back(i * outerSegment + 0);

						indices.push_back(i * outerSegment + j);
						indices.push_back(0 * outerSegment + j);
						indices.push_back(0 * outerSegment + 0);
					}
				}
			}
		}

		//Lines
		for (unsigned int i = 0; i < innerSegment; i++) {
			for (unsigned int j = 0; j < outerSegment; j++) {
				if (i < innerSegment - 1) {
					if (j < outerSegment - 1) {
						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(i * outerSegment + j + 1);
						lineIndices.push_back(i * outerSegment + j + 1);
						lineIndices.push_back((i + 1) * outerSegment + j + 1);

						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back((i + 1) * outerSegment + j + 1);

					}
					else {
						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(i * outerSegment + 0);
						lineIndices.push_back(i * outerSegment + 0);
						lineIndices.push_back((i + 1) * outerSegment + 0);

						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back((i + 1) * outerSegment + 0);
					}
				}
				else {
					if (j < outerSegment - 1) {
						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(i * outerSegment + j + 1);
						lineIndices.push_back(i * outerSegment + j + 1);
						lineIndices.push_back(0 * outerSegment + j + 1);

						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(0 * outerSegment + j + 1);

					}
					else {
						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(i * outerSegment + 0);
						lineIndices.push_back(i * outerSegment + 0);
						lineIndices.push_back(0 * outerSegment + 0);

						lineIndices.push_back(i * outerSegment + j);
						lineIndices.push_back(0 * outerSegment + 0);
					}
				}
			}
		}

		//std::cout << "Vertices size: " << vertices.size() << std::endl;
		//std::cout << "Indices size: " << indices.size() << std::endl;
		//std::cout << "LineIndices size: " << lineIndices.size() << std::endl;

		vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
		ib->subData(0, indices.size() * sizeof(unsigned int), indices.data());
		ibl->subData(0, lineIndices.size() * sizeof(unsigned int), lineIndices.data());
	}

	void Circle3D::setInnerRadius(const float radius) {
		if (radius < outerRadius)
			return;

		if (radius >= 0) {
			irTemp = this->innerRadius = radius;
			vertices.clear();
			vertices.resize(0);
			vertices.shrink_to_fit();

			for (unsigned int i = 0; i < innerSegment; i++) {
				float radY = (2.0f * PI / (float)innerSegment) * (float)i;
				Vec3 iPos;
				iPos.x = cosf(radY) * innerRadius;
				iPos.z = sinf(radY) * innerRadius;
				iPos.y = 0;

				for (unsigned int j = 0; j < outerSegment; j++) {
					float radX = (2.0f * PI / (float)outerSegment) * (float)j;
					Vec3 oPos;
					oPos.y = cosf(radX);
					oPos.x = cosf(radY) * sinf(radX);
					oPos.z = sinf(radY) * sinf(radX);

					vertices.push_back({ iPos.x + oPos.x * outerRadius,iPos.y + oPos.y * outerRadius ,iPos.z + oPos.z * outerRadius });
					vertices.push_back(oPos);
				}
			}

			vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
		}

	}

	float Circle3D::getInnerRadius() {
		return innerRadius;
	}

	void Circle3D::setOuterRadius(const float radius) {
		if (radius > innerRadius)
			return;

		if (radius >= 0) {
			orTemp = this->outerRadius = radius;
			vertices.clear();
			vertices.resize(0);
			vertices.shrink_to_fit();

			for (unsigned int i = 0; i < innerSegment; i++) {
				float radY = (2.0f * PI / (float)innerSegment) * (float)i;
				Vec3 iPos;
				iPos.x = cosf(radY) * innerRadius;
				iPos.z = sinf(radY) * innerRadius;
				iPos.y = 0;

				for (unsigned int j = 0; j < outerSegment; j++) {
					float radX = (2.0f * PI / (float)outerSegment) * (float)j;
					Vec3 oPos;
					oPos.y = cosf(radX);
					oPos.x = cosf(radY) * sinf(radX);
					oPos.z = sinf(radY) * sinf(radX);

					vertices.push_back({ iPos.x + oPos.x * outerRadius,iPos.y + oPos.y * outerRadius ,iPos.z + oPos.z * outerRadius });
					vertices.push_back(oPos);
				}
			}

			vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
		}
	}

	float Circle3D::getOuterRadius() {
		return outerRadius;
	}

	void Circle3D::setRadius(const float inner, const float outer) {
		if (outer > inner)
			return;

		if (inner < 0 && outer < 0)
			return;
		if (inner >= 0)
			irTemp = this->innerRadius = inner;
		if (outer >= 0)
			orTemp = this->outerRadius = outer;

		vertices.clear();
		vertices.resize(0);
		vertices.shrink_to_fit();

		for (unsigned int i = 0; i < innerSegment; i++) {
			float radY = (2.0f * PI / (float)innerSegment) * (float)i;
			Vec3 iPos;
			iPos.x = cosf(radY) * innerRadius;
			iPos.z = sinf(radY) * innerRadius;
			iPos.y = 0;

			for (unsigned int j = 0; j < outerSegment; j++) {
				float radX = (2.0f * PI / (float)outerSegment) * (float)j;
				Vec3 oPos;
				oPos.y = cosf(radX);
				oPos.x = cosf(radY) * sinf(radX);
				oPos.z = sinf(radY) * sinf(radX);

				vertices.push_back({ iPos.x + oPos.x * outerRadius,iPos.y + oPos.y * outerRadius ,iPos.z + oPos.z * outerRadius });
				vertices.push_back(oPos);
			}
		}

		vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
	}

	void Circle3D::setMaterial(Material material) {
		this->material = material;
	}

}

