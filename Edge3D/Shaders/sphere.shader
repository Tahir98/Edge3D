#vertex shader
#version 330 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vNormal;

out vec3 pos;
out vec3 color;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 a = vec4(vPos, 1.0f) * model * view;
    pos = vec3(a.xyz);
    gl_Position = a * projection;
    color = vColor;
    normal = vNormal * mat3(model);
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
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewerPos;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;


void main() {
    vec3 ambient = light.ambient * material.ambient;

    //Diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    //Specular
    vec3 viewerDir = normalize(viewerPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewerDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;

    vec4 col = texture(texture_diffuse1, texCoord);
    Vec4 offsetCol = texture(texture_diffuse1, texCoord - 0.02f);

    if (length(col - offsetCol) > 0.1f)
        col = vec4(0);
        
    color = vec4(result, 1.0) * col;
}
