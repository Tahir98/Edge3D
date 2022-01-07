#include "Renderer.h"

namespace Edge3D {
	void Renderer::render3D(Shader& shader, VertexArray& va, IndexBuffer& ib, Camera& camera, Matrix4x4& model, unsigned int count) {
		shader.bind();
		va.bind();
		ib.bind();

		shader.setUniformMatrix4fv("model", 1, false, model.m[0]);
		shader.setUniformMatrix4fv("view", 1, false, camera.getViewMatrix());
		shader.setUniformMatrix4fv("projection", 1, false, camera.getProjectionMatrix());

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::render3D(Shader& shader, VertexArray& va, IndexBuffer& ib, Camera& camera, Material material, Matrix4x4& model, unsigned int count) {
		shader.bind();
		va.bind();
		ib.bind();

		shader.setUniformMatrix4fv("model", 1, false, model.m[0]);
		shader.setUniformMatrix4fv("view", 1, false, camera.getViewMatrix());
		shader.setUniformMatrix4fv("projection", 1, false, camera.getProjectionMatrix());

		shader.setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
		shader.setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
		shader.setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
		shader.setUniform1f("material.shininess",material.shininess);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

}

