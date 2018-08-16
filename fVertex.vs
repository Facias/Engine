#version 400 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexLight;

uniform sampler2D texture15;

out vec3 normalVarying;
out vec2 uvVarying;
out vec3 posVarying;
out vec3 lightVarying;

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

uniform vec3 cascadeCenter0;
uniform vec3 cascadeCenter1;
uniform vec3 cascadeCenter2;
uniform vec3 cascadeCenter3;

out vec4 v_position;
 out float shadow;
uniform vec3 campos;



float shadowSample(highp vec2 uv, int cascade, vec3 lump)
{
	if (cascade == 0)
	{
		float shadow[36];
		for (int n = 0; n< 36; n++)
			shadow[n] = 1.0;
			
		
		highp float compare = (transformLight0.z + transformLight0.w)/2.0;
		float bias = .00002;	
			
		highp vec4 val[9];
		highp vec2 duv = 1.0/vec2(4096,-1024);
		val[0] = textureLod(texture15, uv, 0.0);

		if (val[0].x < compare -bias) shadow[0] = 0.0;
		
		return mix(1.0,shadow[0], max(0.0, min(1.0,val[0].x-compare))  );//ave[12];
	}
	
	if (cascade == 1)
	{
		float shadow[36];
		for (int n = 0; n< 36; n++)
			shadow[n] = 1.0;
			
		
		highp float compare = (transformLight1.z + transformLight1.w)/2.0;
		float bias = .002;	
			
		highp vec4 val[9];
		highp vec2 duv = 1.0/vec2(4096,-1024);
		val[0] = textureLod(texture15, uv, 0.0);

		if (val[0].x < compare -bias) shadow[0] = 0.0;
		
		return mix(1.0,shadow[0], max(0.0, min(1.0,val[0].x-compare))  );//ave[12];
	}
	
	if (cascade == 2)
	{
		float shadow[36];
		for (int n = 0; n< 36; n++)
			shadow[n] = 1.0;
			
		
		highp float compare = (transformLight2.z + transformLight2.w)/2.0;
		float bias = .0002;	
			
		highp vec4 val[9];
		highp vec2 duv = 1.0/vec2(4096,-1024);
		val[0] = textureLod(texture15, uv, 0.0);

		if (val[0].x < compare -bias) shadow[0] = 0.0;
		
		
		
		return shadow[0];
	}
	
	if (cascade == 3)
	{
		float shadow[36];
		for (int n = 0; n< 36; n++)
			shadow[n] = 1.0;
			
		
		highp float compare = (transformLight3.z + transformLight3.w)/2.0;
		float bias = .0002;	
			
		highp vec4 val[9];
		highp vec2 duv = 1.0/vec2(4096,-1024);
		val[0] = textureLod(texture15, uv, 0.0);

		if (val[0].x < compare -bias) shadow[0] = 0.0;
		
		return lump.x;
	}
}


void main()
{
	vec4 pos =  model*vec4(vertexPosition,1);
	posVarying = pos.xyz;
	
	transformLight0 = MVP_shadow0 * vec4(vertexPosition,1);
	transformLight1 = MVP_shadow1 * vec4(vertexPosition,1);
	transformLight2 = MVP_shadow2 * vec4(vertexPosition,1);
	transformLight3 = MVP_shadow3 * vec4(vertexPosition,1);
	
	
	vec4 norm = transform * vec4(vertexNormal, 1);
	normalVarying = normalize(norm.xyz);
	uvVarying = vertexUV;
	
	lightVarying = vertexLight;
	
	
	highp float val = textureLod(texture15, vec2(.5*(1+transformLight2.xy)/vec2(4,1)+vec2(.5,0)), 1).x;
	
	highp float compare = (transformLight2.z + transformLight2.w)/2.0;
	float bias = .001;	
	
	shadow = 1.0;
	if (val < compare -bias) shadow = 0;
	
	float z_b = val;
	float z_n = 2.0 * z_b - 1.0;
	float zNear = 1.0;
	float zFar = 800.f;
	float realdep0 = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));
	
	
	z_b = compare;
	z_n = 2.0 * z_b - 1.0;
	float realdep1 = 2.0 * zNear * zFar / (zFar + zNear - z_n * (zFar - zNear));	
	

	shadow = mix(1, shadow, (realdep1-realdep0)*50);

	
	shadow = mix(shadow,1.0, max(0.0, min(1.0,(val-(compare-bias))*.25  ))  );//ave[12];
	
	gl_Position = MVP * vec4(vertexPosition, 1);
}