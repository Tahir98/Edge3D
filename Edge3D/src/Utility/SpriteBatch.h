#pragma once
#include "OpenGL/Shader.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Texture.h"

#include <vector>
#include <utility>


namespace Edge3D {
	class SpriteBatch {
	private:
		VertexBuffer vb;
		VertexArray va;
		Shader shader;

		std::vector<float> vertices;
		std::vector<std::pair<int, int>> ids;

		int maxTexSlots;
		int usedTexSlots = 0;
		bool isBegin = false;


		float width, height;
	public:
		SpriteBatch(const float width,const float height);
		~SpriteBatch();

		void begin();
		void end();

		void draw(Texture& texture, const float x, const float y, const float width, const float height);

	private:
		inline void addVertices(const float x, const float y, const float width, const float height, const int slot);

	};
}

