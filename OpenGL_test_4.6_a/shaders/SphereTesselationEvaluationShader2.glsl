#version 400 compatibility

layout( isolines, equal_spacing) in;

patch in float tcRadius;
patch in vec3 tcCenter;

out vec3 teNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const float pi = 3.14159265;

void main( )
{
	vec3 p = gl_in[0].gl_Position.xyz;
	float u0 = gl_TessCoord.x;
	float u;
	float v;
	if (u0<0.5)
	{
	    u = u0*2;
		v = gl_TessCoord.y;
	}
	else
	{
	    u = gl_TessCoord.y;
		v = u0*2;
	}
	float phi = pi * ( u - .5 );
	float theta = 2. * pi * ( v - .5 );
	float cosphi = cos(phi);
	vec3 pos = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );
	teNormal = normalize(pos);
	pos *= ( tcRadius );
	pos += tcCenter;
	gl_Position = projectionMatrix*viewMatrix*modelMatrix* vec4( pos,1. );
}
