#pragma once
#include <vector>
#include "Edge3D.h"
#include "Random/Xorshift.h"

class Terrain {
private:
	struct Vertex {
		Vec3 pos;
		Vec3 normal;
		Vec3 texCoord;
	};

	struct PerlinNoiseLayer {
		int width, height;
		Vec2 offset;
	};

	int width, height;

	std::vector<Vertex> b_vertices; // base vertices
	std::vector<Vertex> f_vertices; // final vertices
	std::vector<uint32_t> indices;
	std::vector<uint32_t> wf_indices;

	Edge3D::VertexBuffer* vb = nullptr;
	Edge3D::VertexArray* va = nullptr;
	Edge3D::IndexBuffer* ib = nullptr;
	Edge3D::IndexBuffer* wf_ib = nullptr;
	Edge3D::Shader* shader = nullptr;
	Edge3D::Texture* texture = nullptr;

	Material material;

	Vec3 scale = { 1,1,1 };
	Vec3 pos = { 0,0,0 };
	Vec3 rotation = { 0,0,0 };

	Xorshift xorshift;
public:
	bool includeTexCoord = false;
	bool wireframe = false;

	Terrain(const unsigned int width, const unsigned int height) : width(width), height(height){
		shader = new Edge3D::Shader("Shaders/terrain.shader");
		material.ambient = { 0.3f,0.3f,0.3f };
		material.diffuse = { 0.5f,0.5f,0.5f };
		material.specular = { 0.7f,0.7f,0.7f };
		material.shininess = 50;
	}

	Terrain(const unsigned int width, const unsigned int height, const bool includeTexCoord) : width(width), height(height), includeTexCoord(includeTexCoord) {
		shader = new Edge3D::Shader("Shaders/terrain.shader");

		material.ambient = { 0.3f,0.3f,0.3f };
		material.diffuse = { 0.5f,0.5f,0.5f };
		material.specular = { 0.7f,0.7f,0.7f };
		material.shininess = 50;
	}

	~Terrain() {
		delete vb;
		delete va;
		delete ib;
		delete shader;

		delete texture;
	}

	void generate() {
		deleteTerrainData();

		calculateVertices();
		calculateIndices();

		vb = new Edge3D::VertexBuffer((float*)f_vertices.data(), sizeof(Vertex) * f_vertices.size(), GL_STATIC_DRAW);
		va = new Edge3D::VertexArray();
		va->bind();
		vb->bind();
		va->addVertexAttribute(vb, 3, GL_FLOAT, false);
		va->addVertexAttribute(vb, 3, GL_FLOAT, false);
		va->addVertexAttribute(vb, 3, GL_FLOAT, false);

		ib = new Edge3D::IndexBuffer(indices.data(), sizeof(unsigned int) * indices.size(), GL_STATIC_DRAW);

		wf_ib = new Edge3D::IndexBuffer(wf_indices.data(), sizeof(unsigned int) * wf_indices.size(), GL_STATIC_DRAW);
	}

	void setTexture(const char* filepath) {
		texture = new Edge3D::Texture(filepath);
	}

	void draw(Edge3D::PerspectiveCamera& camera, DirectionalLight& light) {
		va->bind();
		vb->bind();
		shader->bind();
		ib->bind();

		Matrix4x4 model = Math::scale(scale) * Math::translate(pos);

		//shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
		shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
		shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

		shader->setUniform3f("camPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z );

		shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
		shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
		shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
		shader->setUniform1f("material.shininess", material.shininess);

		shader->setUniform3f("light.direction", light.direction.x, light.direction.y, light.direction.z);
		shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
		shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
		shader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
		shader->setUniform1f("light.intensity", light.intensity);

		shader->setUniform1i("wireframe", 0);
		
		if (includeTexCoord) {
			texture->bind();
			shader->setUniform1i("tex", 0);
			shader->setUniform1i("includeTexture", 1);
		}
		else {
			shader->setUniform1i("includeTexture", 1);
		}
		glEnable(GL_BLEND);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		if (wireframe) {
			glLineWidth(1.2f);
			shader->setUniform1i("wireframe", 1);
			wf_ib->bind();

			glDisable(GL_BLEND);
			glDrawElements(GL_LINES, wf_indices.size(), GL_UNSIGNED_INT, 0);
		}

		int error;
		while((error = glGetError()) != GL_NO_ERROR) {
			APP_LOG_ERROR("OpenGL error: {0}", error);
		}
	}

	Vertex* getVertices() {
		return f_vertices.data();
	}

	unsigned int getVertexCount() {
		return f_vertices.size();
	}

	void setSize(const unsigned int width, const unsigned int height) {
		this->width = width;
		this->height = height;
	}

	unsigned int getWidth() {
		return width;
	}

	unsigned int getHeight() {
		return height;
	}

	Material& getMaterial() {
		return material;
	}

private:
	void deleteTerrainData() {
		delete vb;
		delete va;
		delete ib;
		delete wf_ib;

		b_vertices.clear();
		f_vertices.clear();
		indices.clear();
		wf_indices.clear();
	}

	void calculateVertices() {
		float cornerX = -width / 2.0f;
		float cornerZ = -height / 2.0f;

		std::cout << "CornerX: " << cornerX << std::endl;
		std::cout << "CornerY: " << cornerZ << std::endl;

		for (unsigned int i = 0; i < width; i++) {
			for (unsigned int j = 0; j < height; j++) {
				Vertex vertex;

				float mid = (-cornerX - cornerZ) / 2.0f;
				float x = cornerX + i;
				float z = cornerZ + j;
				float s = mid - sqrtf(x * x + z * z);
				vertex.pos = { x,sinf((x*x + z*z) / 200.0f) * 5.0f, z };
				//vertex.pos = { x,sinf(x / 10.f) * cosf(z / 10.0f) * 10, z };
				vertex.pos.y += (xorshift.nextFloat() * 2.0f - 1) * 0.1f;
				vertex.normal = { 0,1,0 };
				vertex.texCoord = { 0,0,0 };
				b_vertices.push_back(vertex);
				if(!includeTexCoord)
					f_vertices.push_back(vertex);
			}
		}

		calculateNormals();

		for (unsigned int i = 0; i < width; i++) {
			for (unsigned int j = 0; j < height; j++) {
				f_vertices[i * height + j] = b_vertices[i * height + j];
			}
		}

		if (includeTexCoord) {
			for (unsigned int i = 0; i < width; i++) {
				for (unsigned int j = 0; j < height; j++) {
					Vertex vertex;
					vertex = b_vertices[i * height + j];
					f_vertices.push_back(vertex);

					vertex.texCoord = { 0,1,0 };
					f_vertices.push_back(vertex);

					vertex.texCoord = { 1,0,0 };
					f_vertices.push_back(vertex);

					vertex.texCoord = { 1,1,0 };
					f_vertices.push_back(vertex);
				}
			}
		}
		
	}

	void calculateNormals() {
		for (uint32_t i = 0; i < width; i++) {
			for (uint32_t j = 0; j < height; j++) {
				Vertex& v = b_vertices[i * (height) + j];
				Vec3 normal = { 0,0,0 };
				if (i > 0 && i < width -1 && j > 0 && j < height -1) {
					Vertex& v1 = b_vertices[(i - 1) * (height) + j - 1];
					Vertex& v2 = b_vertices[(i - 1) * (height) + j + 1];
					Vertex& v3 = b_vertices[(i + 1) * (height) + j + 1];
					Vertex& v4 = b_vertices[(i + 1) * (height) + j - 1];

					Vertex& v5 = b_vertices[(i - 1) * (height) + j];
					Vertex& v6 = b_vertices[(i) * (height) + j + 1];
					Vertex& v7 = b_vertices[(i + 1) * (height) + j];
					Vertex& v8 = b_vertices[(i) * (height) + j - 1];

					Vec3 c1 = Math::normalize(v.pos - v1.pos);
					Vec3 c2 = Math::normalize(v5.pos - v8.pos);
					normal = normal + Math::crossProduct(c1, c2);

					c1 = Math::normalize(v5.pos - v6.pos);
					c2 = Math::normalize(v2.pos - v.pos);
					normal = normal + Math::crossProduct(c1, c2);

					c1 = Math::normalize(v3.pos - v.pos);
					c2 = Math::normalize(v6.pos - v7.pos);
					normal = normal + Math::crossProduct(c1, c2);

					c1 = Math::normalize(v7.pos - v8.pos);
					c2 = Math::normalize(v.pos - v4.pos);
					normal = normal + Math::crossProduct(c1, c2);
					v.normal = Math::normalize(normal * -1);
					//std::cout << v.normal.x << "   " << v.normal.y << " " << v.normal.z << std::endl;
				}
			}
		}
	}

	void calculateIndices() {
		if (!includeTexCoord) {
			for (uint32_t i = 0; i < width - 1; i++) {
				for (uint32_t j = 0; j < height - 1; j++) {
					indices.push_back(i * (height)+j);
					indices.push_back((i) * (height)+j + 1);
					indices.push_back((i + 1) * (height)+j + 1);

					indices.push_back(i * (height)+j);
					indices.push_back((i + 1) * (height)+j + 1);
					indices.push_back((i + 1) * (height)+j);
					
					wf_indices.push_back(i * (height)+j);
					wf_indices.push_back((i) * (height)+j + 1);

					wf_indices.push_back(i * (height)+j);
					wf_indices.push_back((i + 1) * (height)+j + 1);

					wf_indices.push_back(i * (height)+j);
					wf_indices.push_back((i + 1) * (height)+j);

				}
			}

			
		}
		else {
			for (unsigned int i = 0; i < width - 1; i++) {
				for (unsigned int j = 0; j < height - 1; j++) {
					indices.push_back(i * (height * 4) + (j * 4) + 0);
					indices.push_back(i * (height * 4) + (j * 4 + 4) + 1);
					indices.push_back((i + 1) * (height * 4) + (j * 4 + 4) + 3);

					indices.push_back(i * (height * 4) + (j * 4) + 0);
					indices.push_back((i + 1) * (height * 4) + (j * 4 + 4) + 3);
					indices.push_back((i + 1) * (height * 4) + (j * 4) + 2);

					
					wf_indices.push_back(i * (height * 4) + (j * 4) + 0);
					wf_indices.push_back(i * (height * 4) + (j * 4 + 4) + 1);

					wf_indices.push_back(i * (height * 4) + (j * 4) + 0);
					wf_indices.push_back((i + 1) * (height * 4) + (j * 4 + 4) + 3);

					wf_indices.push_back(i * (height * 4) + (j * 4) + 0);
					wf_indices.push_back((i + 1) * (height * 4) + (j * 4) + 2);
				}
			}
		}
		
	}
};