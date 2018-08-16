
#version 400 core


layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;


out vec2 UV;
out vec2 pos;
uniform vec2 resolution;

void main(){


	vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace*vec2(1,-1)+vec2(0,resolution.y) - (resolution/2); // [0..800][0..600] -> [-400..400][-300..300]
	vertexPosition_homoneneousspace /= (resolution/2);
	gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);
	
	pos = vertexPosition_screenspace;
	UV = vertexUV;
}
