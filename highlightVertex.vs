#version 400 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;


out vec3 normalVarying;
out vec2 uvVarying;
out vec3 posVarying;


uniform mat4 transform;
uniform mat4 MVP;
uniform mat4 model;


uniform mat4 MVP_shadow0;
uniform mat4 MVP_shadow1;
uniform mat4 MVP_shadow2;
uniform mat4 MVP_shadow3;

out vec4 transformLight0;
out vec4 transformLight1;
out vec4 transformLight2;
out vec4 transformLight3;

out vec4 v_position;

void main()
{
	vec4 pos =  model*vec4(vertexPosition,1);
	posVarying = pos.xyz;
	gl_Position = MVP * vec4(vertexPosition, 1);
	transformLight0 = MVP_shadow0 * vec4(vertexPosition,1);
	transformLight1 = MVP_shadow1 * vec4(vertexPosition,1);
	transformLight2 = MVP_shadow2 * vec4(vertexPosition,1);
	transformLight3 = MVP_shadow3 * vec4(vertexPosition,1);
	v_position = gl_Position ;
	
	vec4 norm = transform * vec4(vertexNormal, 1);
	normalVarying = normalize(norm.xyz);
	uvVarying = vertexUV;
	
}