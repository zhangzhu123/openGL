#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 modelMatric;
uniform mat4 viewMatrix;
uniform mat4 perspMatrix;

void main()
{
    gl_Position = perspMatrix * viewMatrix * modelMatric * vec4(position.x, position.y, position.z, 1.0);
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}