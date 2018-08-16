#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 partPosition;
layout(location = 3) in vec3 partScale;
layout(location = 4) in vec3 partColor;

out vec3 normalVarying;
out vec2 uvVarying;
out vec3 posVarying;
out vec3 colorVarying;
uniform mat4 MVP;


void main()
{
	

	vec3 pos ;;
	
	vec3 CameraRight_worldspace = normalize(vec3(MVP[0][0], MVP[1][0], MVP[2][0]));
	vec3 CameraUp_worldspace = normalize(vec3(MVP[0][1], MVP[1][1], MVP[2][1]));
	pos  = partPosition + CameraRight_worldspace*vertexPosition.x*.1*partScale.x + CameraUp_worldspace*vertexPosition.y*.1*partScale.y;
	
	posVarying = pos.xyz;
	colorVarying = partColor;
	
	gl_Position = MVP * vec4(pos,1);

	normalVarying = vec3(.5,.5,1);
	uvVarying = .5*(vertexPosition.xy+1);
}