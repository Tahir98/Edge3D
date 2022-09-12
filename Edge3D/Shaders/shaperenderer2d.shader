#vertex shader
#version 430 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 bufferColor;

out vec4 color;

uniform float width;
uniform float height;


void main() {
	gl_Position = vec4(-2.0f * (width - pos.x) / (width) + 1.0f, -2.0f * (height - pos.y) / (height) + 1.0f, 0.0f, 1.0f);
	color = bufferColor;
}


#fragment shader
#version 430 core

in vec4 color;
out vec4 oColor;

void main() {
	oColor = color;
}