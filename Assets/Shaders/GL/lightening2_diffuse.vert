#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos; 

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 modelInverse;

void main()
{
    gl_Position = mvp * vec4(position, 1.0f);
	FragPos = vec3(model * vec4(position, 1.0f));
	Normal = modelInverse * normal;    
}