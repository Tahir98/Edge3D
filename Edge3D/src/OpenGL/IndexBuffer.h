#pragma once

namespace Edge3D {
	class IndexBuffer {
	private:
		unsigned int id;
		unsigned int count;
	public:
		IndexBuffer(const unsigned int* indices, const unsigned int size, const unsigned int usage);
		~IndexBuffer();

		void subData(const unsigned int index, const unsigned int size, const unsigned int* indices);
		void bind();
		void unbind();
		unsigned int getCount();
	};
}
