#version 410 core
layout (location = 0) in vec3 position;
//layout (location = 1) in vec2 texCoord;

//out vec2 TexCoord;
out vec2 v_rgbNW;
out vec2 v_rgbNE;
out vec2 v_rgbSW;
out vec2 v_rgbSE;
out vec2 v_rgbM;

//uniform vec2 iResolution;
vec2 iResolution = vec2(1024,1024);
out vec2 vUv;

void texcoords(vec2 fragCoord, vec2 resolution,
			out vec2 v_rgbNW, out vec2 v_rgbNE,
			out vec2 v_rgbSW, out vec2 v_rgbSE,
			out vec2 v_rgbM) {
	//vec2 inverseVP = 1.0 / resolution.xy;
	vec2 inverseVP = vec2(1.0 / resolution.x, 1.0 / resolution.y);
	v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
	v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
	v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
	v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
	v_rgbM = vec2(fragCoord * inverseVP);
}

void main()
{
    //gl_Position = vec4(position.x, position.y, position.z, 1.0);
	//TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);

    gl_Position = vec4(position.xy, 0.0, 1.0);
	vUv = (position.xy + 1.0) * 0.5;
	vUv.y = 1.0 - vUv.y;
	vec2 fragCoord = vUv * iResolution;
	texcoords(fragCoord, iResolution, v_rgbNW, v_rgbNE, v_rgbSW, v_rgbSE, v_rgbM);
}