#pragma once
#include "glad/glad.h"
#include <vector>
#include <utility>
#include "VertexBuffer.h"


struct VertexElement{
	unsigned int count;
	unsigned int type;
	bool normalized;

	static unsigned int getTypeSize(unsigned int type) {
		switch (type){
			case GL_FLOAT:
				return 4;
				break;
			case GL_DOUBLE:
				return 8;
				break;
			case GL_UNSIGNED_INT:
				return 4;
				break;
			case GL_INT:
				return 4;
				break;
			case GL_BYTE:
				return 1;
				break;
			case GL_UNSIGNED_BYTE:
				return 1;
				break;
			case GL_SHORT:
				return 2;
				break;
			case GL_UNSIGNED_SHORT:
				return 1;
				break;
			default:
				return -1;
				break;
		}
	}

};

namespace Edge3D {
	class VertexArray {
	private:
		unsigned int id;
		unsigned int stride = 0;
		std::vector<VertexElement> elements;
		VertexBuffer* vb;

	public:
		VertexArray();
		~VertexArray();

		void bind();
		void unbind();

		void addVertexAttribute(VertexBuffer* vb,const unsigned int count, const unsigned int type, const bool normalized);

		unsigned int getCount();
	};
}


