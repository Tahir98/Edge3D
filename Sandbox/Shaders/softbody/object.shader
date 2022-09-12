#vertex shader
#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

uniform mat4 view;
uniform mat4 projection;

out vec3 pos;
out vec3 normal;

void main() {
	pos = vPos;
	normal = vNormal;

	gl_Position = vec4(pos, 1.0f) * view * projection;
}




#fragment shader
#version 430 core

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float intensity;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform DirectionalLight light;
uniform Material material;

uniform vec3 camPos;

in vec3 pos;
in vec3 normal;

out vec4 oColor;

void main() {
	vec3 Normal = normalize(normal);
	vec3 direction = normalize(light.direction);

	//Ambient color
	vec3 ambient = material.ambient * light.ambient;

	//Diffuse color
	float diff = max(dot(Normal,normalize(-direction)),0.0f);
	vec3 diffuse = material.diffuse * light.diffuse * diff;

	//Specular color
	vec3 viewDir = normalize(camPos - pos);
	vec3 reflectDir = normalize(reflect(direction,Normal));
	float spec = pow(max(dot(viewDir,reflectDir),0.0f), material.shininess);

	vec3 specular = material.specular * light.specular * spec;

	//Output color
	oColor = vec4((ambient + diffuse + specular) * light.intensity, 1.0f);
}