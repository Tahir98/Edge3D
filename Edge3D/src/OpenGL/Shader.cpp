#include "Shader.h"
#include "glad/glad.h"
#include "Log.h"
#include <fstream>


namespace Edge3D {
	
	Shader::Shader(const char* path) : filePath(path) {
		id = glCreateProgram();
		compileShader();

	}
	Shader::~Shader() {
		ENG_LOG_INFO("Shader(program) deleted, id:{0}", id);
		glDeleteProgram(id);
	}


	void Shader::bind() {
		glUseProgram(id);
	}
	void Shader::unbind() {
		glUseProgram(0);
	}


	void Shader::compileShader() {

		std::ifstream stream(filePath);
		if (!stream.is_open()) {
			ENG_LOG_ERROR("File not found: {0}", filePath);
		}

		std::string line;
		int type = -1;

		std::string vs, fs;

		while (getline(stream, line)) {
			//ENG_LOG_INFO("{0}",line);

			if (line.find("#vertex shader") != std::string::npos)
				type = 0;
			else if (line.find("#fragment shader") != std::string::npos)
				type = 1;
			else {
				if (type == 0)
					vs += line + "\n";
				else if (type == 1)
					fs += line + "\n";
			}
		}

		const char* vsSource = vs.c_str();
		const char* fsSource = fs.c_str();

		vsID = glCreateShader(GL_VERTEX_SHADER);
		fsID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vsID, 1, &vsSource, nullptr);
		glShaderSource(fsID, 1, &fsSource, nullptr);

		glCompileShader(vsID);
		glCompileShader(fsID);

		int success;
		glGetShaderiv(vsID, GL_COMPILE_STATUS, &success);
		if (!success) {
			int length;
			glGetShaderiv(vsID, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length);
			glGetShaderInfoLog(vsID, length, &length, message);

			ENG_LOG_ERROR("FAILED TO COMPILE VERTEX SHADER: {0}", message);

			free(message);
		}

		glGetShaderiv(fsID, GL_COMPILE_STATUS, &success);
		if (!success) {
			int length;
			glGetShaderiv(fsID, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length);
			glGetShaderInfoLog(fsID, length, &length, message);

			ENG_LOG_ERROR("FAILED TO COMPILE FRAGMENT SHADER: {0}", message);

			free(message);
		}

		glAttachShader(id, vsID);
		glAttachShader(id, fsID);

		glLinkProgram(id);

		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			int length;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length);
			glGetProgramInfoLog(id, length, &length, message);

			ENG_LOG_ERROR("FAILED TO LINK SHADER(PROGRAM): {0}", message);
			free(message);
		}

		glValidateProgram(id);

		glGetProgramiv(id, GL_VALIDATE_STATUS, &success);
		if (!success) {
			int length;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)malloc(length);
			glGetProgramInfoLog(id, length, &length, message);

			ENG_LOG_ERROR("FAILED TO VALIDATE SHADER(PROGRAM): {0}", message);
			free(message);
			exit(-1);
		}

		glDeleteShader(vsID);
		glDeleteShader(fsID);

		ENG_LOG_INFO("Shader(program) created, id:{0}", id);
	}


	void Shader::setUniform1i(const char* name, const int& value) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform1i(location, value);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform2i(const char* name, const int& v1, const int& v2){
		int location = glGetUniformLocation(id, name);
		if(location != -1) 
			glUniform2i(location, v1, v2);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform3i(const char* name, const int& v1, const int& v2, const int& v3){
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform3i(location, v1, v2, v3);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform4i(const char* name, const int& v1, const int& v2, const int& v3, const int& v4){
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform4i(location, v1, v2, v3, v4);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniformiv(const char* name, const unsigned int count, const int* value) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform1iv(location, count, value);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform1f(const char* name, const float& value) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform1f(location, value);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform2f(const char* name, const float& v1, const float& v2) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform2f(location, v1, v2);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform2f(const char* name, const Vec2& vec2) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform2f(location, vec2.x, vec2.y);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform3f(const char* name,const float& v1,const float& v2, const float& v3) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform3f(location, v1, v2, v3);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform3f(const char* name, const Vec3& vec3) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform3f(location, vec3.x, vec3.y, vec3.z);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform4f(const char* name, const float& v1, const float& v2, const float& v3, const float& v4) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform4f(location, v1, v2, v3, v4);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniform4f(const char* name, const Vec4& vec4) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniformMatrix2fv(const char* name, const unsigned int count, const bool transpose, const float* pos) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniformMatrix2fv(location, count, transpose, pos);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniformMatrix3fv(const char* name, const unsigned int count, const bool transpose, const float* pos) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniformMatrix3fv(location, count, transpose, pos);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}

	void Shader::setUniformMatrix4fv(const char* name, const unsigned int count, const bool transpose, const float* pos) {
		int location = glGetUniformLocation(id, name);
		if (location != -1)
			glUniformMatrix4fv(location, count, transpose, pos);
		else
			ENG_LOG_ERROR("{0}: UNIFORM LOCATION NOT FOUND", name);
	}


	void Shader::bindUniformBuffer(const std::string name, const uint32_t bindingIndex) {
		uint32_t index = glGetUniformBlockIndex(id, name.c_str());
		glUniformBlockBinding(id, index, bindingIndex);
	}

	int Shader::getUniformLocation(const char* name){
		int location = glGetUniformLocation(id,name);
		return location;
	}
}













