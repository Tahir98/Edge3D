#include "VertexArray.h"
#include "Log.h"


namespace Edge3D {
	VertexArray::VertexArray() : vb(nullptr) {
		glGenVertexArrays(1, &id);
		ENG_LOG_INFO("Vertex array created id:{0}", id);
	}

	VertexArray::~VertexArray() {
		ENG_LOG_INFO("Vertex array deleted id:{0}", id);
		glDeleteVertexArrays(1, &id);
	}

	void VertexArray::bind() {
		glBindVertexArray(id);
	}

	void VertexArray::unbind() {
		glBindVertexArray(0);
	}

	void VertexArray::addVertexAttribute(VertexBuffer* vb,const unsigned int count, const unsigned int type, const bool normalized) {
		if (this->vb != nullptr && vb != this->vb) {
			ENG_LOG_ERROR("Only one vertex buffer allowed, Vertex Array id: {0}",id);
			return;
		}
		else if(this->vb == nullptr && vb != nullptr) {
			this->vb = vb;
		}
		else if(vb == nullptr) {
			ENG_LOG_ERROR("Vertex buffer object is null, Vertex Array id: {0}", id);
		}

		bind();
		vb->bind();

		if (count > 0 && VertexElement::getTypeSize(type) != -1) {
			stride += count * VertexElement::getTypeSize(type);
			uint16_t offset = 0;
			elements.push_back({ count , type , normalized });
			bind();

			for (unsigned int i = 0; i < elements.size(); i++) {
				glVertexAttribPointer(i, elements[i].count, elements[i].type, elements[i].normalized, stride, (const void*)offset);
				glEnableVertexAttribArray(i);
				offset += elements[i].count * VertexElement::getTypeSize(elements[i].type);
			}
		}
	}

	unsigned int VertexArray::getCount() {
		if (vb == nullptr)
			return 0;
		else
			return vb->getCount();

	}
}

