#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D screenTexture;

void main()
{
    
    vec3 texColor = texture(screenTexture, TexCoords).rgb;

    color = vec4(texColor,1);
	//color = vec4(0.3,0.3,0.3,1);    

} 
