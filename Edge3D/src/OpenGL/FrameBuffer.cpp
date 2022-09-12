#include "FrameBuffer.h"
#include "glad/glad.h"
#include "Log.h"

namespace Edge3D {
	FrameBuffer::FrameBuffer(uint32_t width, uint32_t height): width(width),height(height) {
		//Frame buffer generation
		glGenFramebuffers(1, &frameBufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

		//Texture generation
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		//Attaching texture to the frame buffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		//Render buffer generation
		glGenRenderbuffers(1, &renderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		//Attaching render buffer to the frame buffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);

		//Check if frame buffer is completed
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			ENG_LOG_ERROR("Frame buffer error: frame bufffer is not completed");
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FrameBuffer::~FrameBuffer() {
		glDeleteTextures(1, &textureID);
		glDeleteRenderbuffers(1, &renderBufferID);
		glDeleteFramebuffers(1, &frameBufferID);
	}


	void FrameBuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	}
	
	void FrameBuffer::unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t& FrameBuffer::getTextureID() {
		return textureID;
	}
	uint32_t& FrameBuffer::getRenderBufferID() {
		return renderBufferID;
	}

	void FrameBuffer::clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::clearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void FrameBuffer::resize(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;

		//Resizing color texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		//resizing depth and stencil buffer 
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	uint32_t FrameBuffer::getWidth() {
		return width;
	}
	
	uint32_t FrameBuffer::getHeight() {
		return height;
	}
}