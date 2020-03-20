#ifdef COMPILING_VS

layout (location = 0) in vec4 position;

out vec3 vCenter;
out float vRadius;

void main( )
{
	vCenter = position.xyz;
	vRadius = position.w;
	gl_Position = position;
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

uniform int maxTessellationLevel = 64;
uniform int minTessellationLevel = 4;

uniform int pixelWidth = 1920;

uniform float desiredPixelsPerTriangle = 20;

vec3 wc2ndc(vec3 wc)
{
	vec4 cc = projectionMatrix*vec4(wc,1);
	return cc.xyz / cc.w;
}

float calculateTessellationLevel(vec3 center, float radius)
{
	vec3 centerVC = (viewMatrix*modelMatrix*vec4(center,1)).xyz;
	vec3 p0NDC = wc2ndc(centerVC + vec3(-radius,0,0));
	vec3 p1NDC = wc2ndc(centerVC + vec3(+radius,0,0));
	float clipCoordinateDiameter = length(p0NDC - p1NDC);
	float pixelDiameter = clipCoordinateDiameter * pixelWidth / 2;

	return clamp(pixelDiameter/desiredPixelsPerTriangle,minTessellationLevel,maxTessellationLevel);
}

void main( )
{
	vec3 center = vCenter[0];
	float radius = vRadius[0];

	tcCenter = center;
	tcRadius = radius;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; 
	
	float tessellationLevel = calculateTessellationLevel(center, radius);

	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
}


#endif

#ifdef COMPILING_TES

layout( isolines, equal_spacing) in;

patch in float tcRadius;
patch in vec3 tcCenter;

out vec3 teNormal;
out vec4 teColor;

uniform mat4 modelMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 viewMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 projectionMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

const float pi = 3.14159265;

void main( )
{
	float u0 = gl_TessCoord.y;
	float v0 = gl_TessCoord.x;

	bool h = (u0<0.5);
	float u = h ? u0*2 : v0;
	float v = h ? v0   : 2*u0-1;
	float phi = pi * ( u - .5 );
	float theta = 2. * pi * ( v - .5 );
	float cosphi = cos(phi);
	vec3 pos = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );

	teNormal = normalize(pos);
	teColor = (u0<0.5) ? vec4(1,0,0,1) : vec4(0,0,1,0);
	pos *= tcRadius;
	pos += tcCenter;
	gl_Position = projectionMatrix*viewMatrix*modelMatrix* vec4( pos,1. );
}

#endif

#ifdef COMPILING_FS

in vec3 teNormal;
in vec4 teColor;

out vec4 frag_color;

void main() {
  frag_color = teColor;
}

#endif
