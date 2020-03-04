#version 400 compatibility
layout( quads, equal_spacing, ccw) in;

patch in float tcRadius;
patch in vec3 tcCenter;

out vec3 teNormal;

uniform float uScale;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const float pi = 3.14159265;

void main( )
{
	vec3 p = gl_in[0].gl_Position.xyz;
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	float w = gl_TessCoord.z;
	float phi = pi * ( u - .5 );
	float theta = 2. * pi * ( v - .5 );
	float cosphi = cos(phi);
	vec3 xyz = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );
	teNormal = xyz;
	xyz *= ( uScale * tcRadius );
	xyz += tcCenter;
	gl_Position = projectionMatrix*viewMatrix*modelMatrix* vec4( xyz,1. );
}