
#vertex shader
#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTexCoord;


out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float chosen;

void main() {
	normal = vNormal * mat3(model);
	texCoord = vec2(vTexCoord.xy);

	vec4 pos = vec4(vPos, 1.0f) * model;
	fragPos = vec3(pos.xyz);

	pos = pos * view * projection;

	if (chosen == 0.0) {
		gl_PointSize = 5.0;
	}
	else if (chosen == 1.0) {
		pos.z = -pos.w;
	}
	
	gl_Position = pos;
}





#fragment shader
#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadrantic;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;
out vec4 o_color;

uniform float chosen;
uniform sampler2D tex;
uniform int drawTexture;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
	if (chosen == 0.0)
		o_color = vec4(0, 0, 0, 1);
	else if(chosen == 1.0)
		o_color = vec4(1,0,0,1);
	else if (chosen == 2.0) {
		if (light.specular.x != 0.0) {
			// Ambient
			vec3 ambient = light.ambient * material.ambient;

			// Diffuse 
			vec3 norm = normalize(normal);
			vec3 lightDir = normalize(light.pos - fragPos);
			//lightDir.z *= -1.0;
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = light.diffuse * (diff * material.diffuse);

			// Specular
			vec3 viewDir = normalize(viewPos - fragPos);
			vec3 reflectDir = normalize(2.0 * (dot(lightDir, norm)) * norm - lightDir);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
			vec3 specular = light.specular * (spec * material.specular);

			//Attenuation
			float distance = length(light.pos - fragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadrantic * (distance * distance));

			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;

			vec3 result = ambient + diffuse + specular;

			if (drawTexture == 1) {
				vec4 col = texture(texture_diffuse1, texCoord);
				vec4 offsetCol = texture(texture_diffuse1, texCoord - 0.02f);

				if (length(col - offsetCol) > 0.1f)
					col = vec4(0);

				o_color = vec4(result, 1.0) * col;
			}
			else {
				vec4 col = texture(texture_diffuse1, texCoord);
				vec4 offsetCol = texture(texture_diffuse1, texCoord - 0.02f);

				if (length(col - offsetCol) > 0.1f)
					col = vec4(0);

				o_color = col;
			}
				
		}
		else {
			if (drawTexture == 1)
				vec4 col = texture(texture_diffuse1, texCoord);
			vec4 offsetCol = texture(texture_diffuse1, texCoord - 0.02f);

			if (length(col - offsetCol) > 0.1f)
				col = vec4(0);

			o_color = col;
			else
				o_color = vec4(material.diffuse,1.0);
		}
		
	}
}
