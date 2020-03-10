#ifdef COMPILING_VS

layout (location = 0) in vec4 position;

out vec3 vCenter;
out float vRadius;

void main( )
{
	vCenter = position.xyz;
	vRadius = position.w;
	gl_Position = vec4( 0., 0., 0., 1. );
}

#endif

#ifdef COMPILING_TCS

in float vRadius[ ];
in vec3 vCenter[ ];

patch out float tcRadius;
patch out vec3 tcCenter;
layout( vertices = 1 ) out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float uDetail;

uniform int maxTessellationLevel;

uniform int pixelWidth;

const float desiredPixelsPerTriangle = 10.0f;

void main( )
{
	vec3 center = vCenter[0];
	float radius = vRadius[0];
	vec3 centerInViewCoordinates = (viewMatrix*modelMatrix*vec4(center,1)).xyz;
	vec3 p0 = centerInViewCoordinates + vec3(-radius,0,0);
	vec3 p1 = centerInViewCoordinates + vec3( radius,0,0);
	vec4 p0CC = projectionMatrix*vec4(p0,1);
	vec4 p1CC = projectionMatrix*vec4(p1,1);
	vec3 p0NDC = p0CC.xyz / p0CC.w;
	vec3 p1NDC = p1CC.xyz / p1CC.w;
	float clipCoordinateDiameter = length(p0NDC - p1NDC);
	float pixelDiameter = clipCoordinateDiameter * pixelWidth;

	float tessellationLevel = clamp(pixelDiameter/desiredPixelsPerTriangle,8,maxTessellationLevel);

	tcCenter = center;
	tcRadius = radius;
	gl_out[ gl_InvocationID ].gl_Position = gl_in[ 0 ].gl_Position; 
	
	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
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
