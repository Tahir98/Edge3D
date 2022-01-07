#pragma once
#include "Edge3D.h"
#include <vector>
#include <stdlib.h>

class ConvexPolygon : public Edge3D::Scene {
private:
	Edge3D::Window& window;

	struct Vertex {
		Vec3 pos;
		Vec3 color;
	};

	struct Polygon {
		Vec2 pos;
		std::vector<Vec2> nodes;
		bool collide = false;
	};



	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Polygon> polygons;

	Edge3D::InputHandler handler;
	Edge3D::VertexBuffer vb;
	Edge3D::VertexArray va;
	Edge3D::IndexBuffer ib;
	Edge3D::Shader shader;
	Vec3 color;
	Matrix3x3 projection;
	float pSize = 10;

	float width, height;

	float speed = 200.0f;

	int chosen = 1;

	float gravity = -300.0f;
public:
	ConvexPolygon(Edge3D::Window& window, std::string name) : window(window), Scene(name), vb(nullptr, 10000, GL_DYNAMIC_DRAW), ib(nullptr, 10000, GL_DYNAMIC_DRAW), shader("Shaders/Line2D.shader") {
		color = { 1,1,1 };

		width = window.getWidth();
		height = window.getHeight();

		va.bind();
		vb.bind();
		va.addVertexAttribute(&vb, 3, GL_FLOAT, false);
		va.addVertexAttribute(&vb, 3, GL_FLOAT, false);

		for (unsigned int i = 0; i < 10; i++) {
			Polygon p;
			Vec2 pos = {(float)(rand() % 1800),(float)(rand() % 1000)};
			p.pos = pos;
			for (unsigned int j = 0; j < 20; j++) {
				p.collide = true;
				p.nodes.push_back({ pos.x + 100.0f * cosf((float)j * PI / 10.0f) ,pos.y + 100.0f * sinf((float)j * PI / 10.0f) });
			}
			polygons.push_back(p);
		}

		Polygon p;
		p.nodes.push_back({ 100,100 });
		p.nodes.push_back({ 1920,100 });
		p.nodes.push_back({ 1920,200 });
		p.nodes.push_back({ 100,200 });
		p.pos = {960,150};
		p.collide = false;
		polygons.push_back(p);

		projection = {
			1.0f / (window.getWidth() / 2.0f), 0.0f,0.0f,
			0.0f,1.0f / (window.getHeight() / 2.0f),0.0f,
			-1.0f,-1.0f,0.0
		};
	}

	~ConvexPolygon() {

	}

	void onCreate() {
		glPointSize(pSize);
		glLineWidth(3.0f);
	}

	void onUpdate(float delta) {

		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL Error, Error code: {0}", error);
		}

		input(delta);
		
		for (Polygon& p : polygons) {
			if (p.collide) {
				p.pos.y += gravity * delta;
				for (Vec2& node : p.nodes)
					node.y += gravity * delta;
			}
		}

		for (unsigned int i = 0; i < polygons.size() - 1; i++) {
			for (unsigned int j = i + 1; j < polygons.size(); j++) {
				collision(&polygons[i],&polygons[j]);
			}
		}

		vb.bind();
		ib.bind();
		va.bind();
		shader.bind();

		vertices.clear();
		indices.clear();



		for (unsigned int a = 0; a < polygons.size(); a++) {
			unsigned int index = vertices.size();
			for (Vec2& node : polygons[a].nodes) {
				if (!polygons[a].collide)
					vertices.push_back({ { node.x,node.y,1.0f }, color });
				else
					vertices.push_back({ { node.x,node.y,1.0f }, {0,0,0 } });

			}
			for (unsigned int i = 0; i < vertices.size() - index; i++) {
				indices.push_back(index + i % (vertices.size() - index));
				indices.push_back(index + (i + 1) % (vertices.size() - index));
			}
		}
		

		

		vb.subData(0, sizeof(Vertex) * vertices.size(), (float*)vertices.data());
		ib.subData(0, sizeof(unsigned int) * indices.size(), indices.data());

		shader.setUniformMatrix3fv("projection", 1, true, projection.m[0]);

		shader.setUniform1i("point", 0);
		glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
		shader.setUniform1i("point", 1);
		glDrawArrays(GL_POINTS, 0, vertices.size());
	}

	void input(float delta) {
		if (handler.getKeyState(GLFW_KEY_1))
			chosen = 1;
		else if (handler.getKeyState(GLFW_KEY_2))
			chosen = 2;

		Polygon& p = chosen == 1 ? polygons[0] : polygons[1];


		if (handler.getKeyState(GLFW_KEY_W)) {
			p.pos.y += speed * delta;
			for (Vec2& node : p.nodes)
				node.y += speed * delta;
		}
		else if (handler.getKeyState(GLFW_KEY_S)) {
			p.pos.y -= speed * delta;
			for (Vec2& node : p.nodes)
				node.y -= speed * delta;
		}

		if (handler.getKeyState(GLFW_KEY_A)) {
			p.pos.x -= speed * delta;
			for (Vec2& node : p.nodes)
				node.x -= speed * delta;
		}
		else if (handler.getKeyState(GLFW_KEY_D)) {
			p.pos.x += speed * delta;
			for (Vec2& node : p.nodes)
				node.x += speed * delta;
		}

		if (handler.getKeyState(GLFW_KEY_Q)) {
			for (Vec2& node : p.nodes) {
				Vec2 temp = node - p.pos;
				node.x = p.pos.x + cosf(PI / 3.0f * delta) * temp.x - sinf(PI / 3.0f * delta) * temp.y;
				node.y = p.pos.y + sinf(PI / 3.0f * delta) * temp.x + cosf(PI / 3.0f * delta) * temp.y;
			}
		}
		else if (handler.getKeyState(GLFW_KEY_E)) {
			for (Vec2& node : p.nodes) {
				Vec2 temp = node - p.pos;
				node.x = p.pos.x + cosf(-PI / 3.0f * delta) * temp.x - sinf(-PI / 3.0f * delta) * temp.y;
				node.y = p.pos.y + sinf(-PI / 3.0f * delta) * temp.x + cosf(-PI / 3.0f * delta) * temp.y;
			}
		}
	}

	void collision(Polygon* p1, Polygon* p2) {
		Polygon* poly1 = p1;
		Polygon* poly2 = p2;

		float overlap = INFINITY;
		Vec2 norm;
		int order;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				poly1 = p2;
				poly2 = p1;
			}

			for (int a = 0; a < poly1->nodes.size(); a++)
			{
				int b = (a + 1) % poly1->nodes.size();
				Vec2 axisProj = { -(poly1->nodes[b].y - poly1->nodes[a].y), poly1->nodes[b].x - poly1->nodes[a].x };
				axisProj = Math::normalize(axisProj);

				// Optional normalisation of projection axis enhances stability slightly
				//float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
				//axisProj = { axisProj.x / d, axisProj.y / d };

				// Work out min and max 1D points for r1
				float min_r1 = INFINITY, max_r1 = -INFINITY;
				for (int p = 0; p < poly1->nodes.size(); p++)
				{
					float q = (poly1->nodes[p].x * axisProj.x + poly1->nodes[p].y * axisProj.y);
					min_r1 = std::min(min_r1, q);
					max_r1 = std::max(max_r1, q);
				}

				// Work out min and max 1D points for r2
				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int p = 0; p < poly2->nodes.size(); p++)
				{
					float q = (poly2->nodes[p].x * axisProj.x + poly2->nodes[p].y * axisProj.y);
					min_r2 = std::min(min_r2, q);
					max_r2 = std::max(max_r2, q);
				}

				// Calculate actual overlap along projected axis, and store the minimum
				if (std::min(max_r1, max_r2) - std::max(min_r1, min_r2) < overlap) {
					overlap = std::min(max_r1, max_r2) - std::max(min_r1, min_r2);
					norm = axisProj;
					order = shape;
				}

				if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
					return;
			}
		}

		// If we got here, the objects have collided, we will displace r1
		// by overlap along the vector between the two object centers
		if (p2->collide) {
			//Vec2 d = { p2->pos.x - p1->pos.x, p2->pos.y - p1->pos.y };
			//float s = sqrtf(d.x * d.x + d.y * d.y);
			//p1->pos.x -= overlap * d.x / s * 0.5f;
			//p1->pos.y -= overlap * d.y / s * 0.5f;
			//for (Vec2& node : p1->nodes) {
			//	node.x -= overlap * d.x / s * 0.5f;
			//	node.y -= overlap * d.y / s * 0.5f;
			//}
			if (order == 0) {
				p1->pos.x -= overlap * (norm.x * 0.5f);
				p1->pos.y -= overlap * (norm.y * 0.5f);
				for (Vec2& node : p1->nodes) {
					node.x -= overlap * norm.x * 0.5f;
					node.y -= overlap * norm.y * 0.5f;
				}

				p2->pos.x += overlap * (norm.x * 0.5f);
				p2->pos.y += overlap * (norm.y * 0.5f);
				for (Vec2& node : p2->nodes) {
					node.x += overlap * norm.x * 0.5f;
					node.y += overlap * norm.y * 0.5f;
				}
			}
			else if (order == 1) {
				p1->pos.x += overlap * (norm.x * 0.5f);
				p1->pos.y += overlap * (norm.y * 0.5f);
				for (Vec2& node : p1->nodes) {
					node.x += overlap * norm.x * 0.5f;
					node.y += overlap * norm.y * 0.5f;
				}

				p2->pos.x -= overlap * (norm.x * 0.5f);
				p2->pos.y -= overlap * (norm.y * 0.5f);
				for (Vec2& node : p2->nodes) {
					node.x -= overlap * norm.x * 0.5f;
					node.y -= overlap * norm.y * 0.5f;
				}
			}
			


			//p2->pos.x += overlap * d.x / s * 0.5f;
			//p2->pos.y += overlap * d.y / s * 0.5f;
			//for (Vec2& node : p2->nodes) {
			//	node.x += overlap * d.x / s * 0.5f;
			//	node.y += overlap * d.y / s * 0.5f;
			//}
		}
		else {
			//Vec2 d = { (0), p2->pos.y - p1->pos.y };
			//float s = sqrtf(d.x * d.x + d.y * d.y);
			//p1->pos.x -= overlap * d.x / s;
			//p1->pos.y -= overlap * d.y / s;
			//for (Vec2& node : p1->nodes) {
			//	node.x -= overlap * d.x / s;
			//	node.y -= overlap * d.y / s;
			//}

			p1->pos.x += overlap * (norm.x);
			p1->pos.y += overlap * (norm.y);
			for (Vec2& node : p1->nodes) {
				node.x += overlap * norm.x;
				node.y += overlap * norm.y;
			}
		}
		
	}

	void onSuspend() {

	}

	void onResume() {

	}

	void onDestroy() {

	}

};

/*Polygon* s1 = p1, * s2 = p2;
	float overlap = INFINITY;
	Vec2 norm;
	int type = 0;

	for (int i = 0; i < 2; i++) {
		if (i == 1) {
			s1 = p2;
			s2 = p1;
		}

		for (int j = 0; j < s1->nodes.size(); j++) {
			Vec2 edge = s1->nodes[(j + 1) % s1->nodes.size()] - s1->nodes[j];
			Vec2 normal = Vec2(edge.y, -edge.x);
			//normal = Math::normalize(normal);
			Vec2 mid = (s1->nodes[(j + 1) % s1->nodes.size()] + s1->nodes[j]) * 0.5f;

			float aMin = INFINITY, aMax = -INFINITY;
			float bMin = INFINITY, bMax = -INFINITY;

			for (int a = 0; a < s1->nodes.size(); a++) {
				float dot = Math::dotProduct(normal, s1->nodes[a] - mid);
				aMin = std::min(dot, aMin);
				aMax = std::max(dot, aMax);
			}

			for (int b = 0; b < s2->nodes.size(); b++) {
				float dot = Math::dotProduct(normal, s2->nodes[b] - mid);
				bMin = std::min(dot, bMin);
				bMax = std::max(dot, bMax);
			}

			if (!(bMax >= aMin && aMax >= bMin)) {
				s1->collide = s1->collide ? true : false;
				s2->collide = s2->collide ? true : false;
				return;
			}

			if (std::min(aMax, bMax) - std::max(aMin, bMin) < overlap) {
				overlap = std::min(aMax, bMax) - std::max(aMin, bMin);
				norm = normal;
				type = i;
			}
		}
	}

	norm = Math::normalize(norm);

	p1->pos = p1->pos - norm * (overlap / 2.0f);
	for (Vec2& node : p1->nodes) {
		node = node - norm * (overlap / 2.0f);
	}

	p2->pos = p2->pos + norm * (overlap / 2.0f);
	for (Vec2& node : p2->nodes) {
		node = node + norm * (overlap / 2.0f);
	}*/