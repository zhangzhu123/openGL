#version 330 core
//layout (location = 0) in vec3 vertex;
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoords;

uniform vec3 eye; 
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;
out vec3 initialRay;

void main() {
    vec2 percent = vertex.xy * 0.5 + 0.5;
    initialRay = mix(mix(ray00, ray01, percent.y), mix(ray10, ray11, percent.y), percent.x);
	//initialRay = mix(mix(vec3(-1,-1,0.5), vec3(-1,1,0.5), percent.y), mix(vec3(1,-1,0.5), vec3(1,1,0.5), percent.y), percent.x);
    //initialRay = vec3(-0.1,-0.2,-0.0);
	gl_Position = vec4(vertex, 0, 1.0);
}