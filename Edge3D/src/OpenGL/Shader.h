#pragma once
#include <vector>
#include <utility>
#include <string>
#include "Math/Math.h"

namespace Edge3D {
	class Shader{
	private:
		unsigned int id;
		unsigned int vsID, fsID;
		const char* filePath;

	public:
		Shader(const char* path);
		~Shader();
		void bind();
		void unbind();
	
		//uniform functions pass values to uniform variales in shaders
		void setUniform1i(const char* name, const int& value);
		void setUniform2i(const char* name, const int& v1, const int& v2);
		void setUniform3i(const char* name, const int& v1, const int& v2, const int& v3);
		void setUniform4i(const char* name, const int& v1, const int& v2, const int& v3, const int& v4);

		void setUniformiv(const char* name, const unsigned int count, const int* value);
		void setUniform1f(const char* name, const float& value);
		void setUniform2f(const char* name, const float& v1, const float& v2);
		void setUniform2f(const char* name, const Vec2& vec2);
		void setUniform3f(const char* name, const float& v1, const float& v2, const float& v3);
		void setUniform3f(const char* name, const Vec3& vec3);
		void setUniform4f(const char* name, const float& v1, const float& v2, const float& v3, const float& v4);
		void setUniform4f(const char* name, const Vec4& vec4);
		void setUniformMatrix2fv(const char* name, const unsigned int count, const bool transpose, const float* pos);
		void setUniformMatrix3fv(const char* name, const unsigned int count, const bool transpose, const float* pos);
		void setUniformMatrix4fv(const char* name, const unsigned int count, const bool transpose, const float* pos);

		void bindUniformBuffer(const std::string name,const uint32_t bindingIndex);
		int getUniformLocation(const char* name);

	private:
		void compileShader();
	};
}


