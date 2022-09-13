#vertex shader
#version 430 core

layout(location = 0) in vec3 vPos;

uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = vec4(vPos, 1.0f) * view * projection;

	gl_Position.z -= 0.0001f;
}




#fragment shader
#version 430 core


out vec4 color;

void main() {
	color = vec4(0, 0, 0, 1);
}