#pragma once
namespace Edge3D {
	class Texture {
	private:
		unsigned int id;
		const char* filePath;
		int width, height;
		int bpp;

	public:
		Texture(const char* path);
		~Texture();

		int getWidth();
		int getHeight();
		int getBPP();

		void bind(const unsigned int slot = 0);
		void unbind();

		unsigned int getId();
	};
}
