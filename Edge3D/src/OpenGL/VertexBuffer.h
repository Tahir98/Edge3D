#pragma once

namespace Edge3D {
	class VertexBuffer {
	private:
		unsigned int id;
		unsigned int count;
	public:
		VertexBuffer(const float* vertices, const unsigned int size, const unsigned int usage);
		~VertexBuffer();

		void subData(const unsigned int index, const unsigned int size, const float* vertices);

		void bind();
		void unbind();
		unsigned int getCount();
	};
}

