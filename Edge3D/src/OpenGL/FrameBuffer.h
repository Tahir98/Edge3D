#pragma once
#include <cstdint>

namespace Edge3D {
	class FrameBuffer {
	private:
		uint32_t frameBufferID;
		uint32_t textureID;
		uint32_t renderBufferID;

		uint32_t width, height;
	public:
		FrameBuffer(uint32_t width, uint32_t height);
		~FrameBuffer();

		void bind();
		void unbind();
		
		uint32_t& getTextureID();
		uint32_t& getRenderBufferID();
		
		void clear();
		void clearColor(float r, float g, float b, float a);
		
		void resize(uint32_t width, uint32_t height);

		uint32_t getWidth();
		uint32_t getHeight();
	};
}

