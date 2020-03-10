#ifdef COMPILING_VS

layout (location = 0) in vec4 position;

out vec3 vCenter;
out float vRadius;

void main( )
{
	vCenter = position.xyz;
	vRadius = position.w;
	gl_Position = vec4( 0., 0., 0., 1. ); // doesn’t matter now – we will in the cords later
}

#endif

#ifdef COMPILING_TCS

in float vRadius[ ];
in vec3 vCenter[ ];

patch out float tcRadius;
patch out vec3 tcCenter;
layout( vertices = 1 ) out;

uniform float uDetail;

void main( )
{
	gl_out[ gl_InvocationID ].gl_Position = gl_in[ 0 ].gl_Position; 
	tcCenter = vCenter[ 0 ];
	tcRadius = vRadius[ 0 ];
	gl_TessLevelOuter[0] = tcRadius * uDetail;
	gl_TessLevelOuter[1] = tcRadius * uDetail;
	gl_TessLevelOuter[2] = tcRadius * uDetail;
}


#endif

#ifdef COMPILING_TES

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

#endif

#ifdef COMPILING_FS

out vec4 frag_color;

void main() {
  frag_color = vec4(1,1,1,1);
}

#endif
