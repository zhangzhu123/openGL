#version 330 core
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture1;
void main()
{
    color = vec4(texture(ourTexture1, TexCoord).xyz, 1.0f);
	//color = vec4(0.0f,0.0f,0.0f,1.0f);
}