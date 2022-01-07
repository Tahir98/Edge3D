#pragma once

#include "Shader.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Utility/Utility.h"
#include <vector>
#include "Utility/Camera.h"

namespace Edge3D {
	class Renderer {
	private:

	public:
		static void render3D(Shader& shader, VertexArray& va, IndexBuffer& ib, Camera& camera, Matrix4x4& model, unsigned int count = 0);
		static void render3D(Shader& shader, VertexArray& va, IndexBuffer& ib, Camera& camera, Material material,Matrix4x4& model, unsigned int count = 0);
	};

}