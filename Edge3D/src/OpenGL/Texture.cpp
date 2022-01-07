#include "Texture.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "Log.h"


namespace Edge3D {
	Texture::Texture(const char* path) : filePath(path), width(0), height(0), bpp(0), id(0) {

		stbi_set_flip_vertically_on_load(true);
		unsigned char* image = stbi_load(filePath, &width, &height, &bpp, 4);

		if (image == nullptr) {
			ENG_LOG_ERROR("Image not found!, path:{0}", path);
		}
		else {
			glGenTextures(1, &id);

			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			ENG_LOG_INFO("Texture created, id:{0}, width:{1}, Height:{2}, BPP:{3}", id, width, height, bpp);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(image);
		}
	}

	Texture::~Texture() {
		ENG_LOG_INFO("Texture deleted id:{0}", id);
		glDeleteTextures(1, &id);
	}

	int Texture::getWidth() {
		return width;
	}

	int Texture::getHeight() {
		return height;
	}

	int Texture::getBPP(){
		return bpp;
	}

	void Texture::bind(const unsigned int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture::unbind() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int Texture::getId() {
		return id;
	}
}

