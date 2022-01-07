#pragma once
#include "IndexBuffer.h"
#include "glad/glad.h"
#include "Log.h"

namespace Edge3D {
	IndexBuffer::IndexBuffer(const unsigned int* indices, const unsigned int size, const unsigned int usage) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, usage);

		count = size / sizeof(unsigned int);

		ENG_LOG_INFO("Index buffer created, id: {0}",id);
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &id);
		ENG_LOG_INFO("Index buffer deleted, id: {0}", id);
	}

	void IndexBuffer::subData(const unsigned int index, const unsigned int size, const unsigned int* indices) {
		bind();
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index, size, indices);
	}

	void IndexBuffer::bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void IndexBuffer::unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	unsigned int IndexBuffer::getCount(){
		return count;
	}
}

