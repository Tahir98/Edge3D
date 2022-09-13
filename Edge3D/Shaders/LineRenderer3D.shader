#vertex shader
#version 430 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 color;

void main() {
	color = vColor;

	gl_Position = vec4(vPos, 1.0f) * view * projection;
	gl_Postion.z -= 0.0001f;
}

#fragment shader
#version 430 core

in vec3 color;

out vec4 fragColor;

void main() {
	fragColor = vec4(color, 1.0f);
}