
#version 400 core

layout(location = 0) in vec2 vertPos;
layout(location = 1) in vec2 vertexUV;


out vec2 UV;
out vec2 pos;

uniform vec2 resolution;
uniform vec2 position;
uniform float angle;
uniform vec2 size;

void main()
{

	mat3 rot = mat3(cos(angle), -sin(angle),0,
					sin(angle), cos(angle),0,
					0,0,1);
	
	// Output position of the vertex, in clip space
	vec3 tpos = rot*vec3(vertPos,1);
	vec2 vertexPosition_homoneneousspace = size*tpos.xy + position*vec2(1,-1)+vec2(0,resolution.y) - resolution/2; 
	vertexPosition_homoneneousspace /= (resolution/2.0);
	gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);
	
	pos = size*tpos.xy*vec2(1,-1) + position;
	UV = vertexUV;
}
