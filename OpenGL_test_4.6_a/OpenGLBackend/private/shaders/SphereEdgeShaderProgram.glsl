uniform mat4 modelMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 viewMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 projectionMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

#ifdef COMPILING_VS

layout (location = 0) in vec4 position;

void main( )
{
	gl_Position = position;
}

#endif

#ifdef COMPILING_TCS

layout( vertices = 1 ) out;

out float tcTesselationLevel[];

uniform int minTessellationLevel = 4;
uniform int maxTessellationLevel = 64;

uniform int pixelWidth = 1920;

uniform float desiredPixelsPerTriangle = 10;

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
	vec4 inPosition = gl_in[gl_InvocationID].gl_Position; 
	vec3 center = inPosition.xyz;
	float radius = inPosition.w;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; 
	
	float tessellationLevel = calculateTessellationLevel(center, radius) * 2;

	tcTesselationLevel[gl_InvocationID] = tessellationLevel;
	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
}


#endif

#ifdef COMPILING_TES

layout(isolines, equal_spacing) in;

in float tcTesselationLevel[];

out vec3 teNormal;
out vec4 teColor;

const float pi = radians(180);

void main( )
{
	vec4 inPosition = gl_in[0].gl_Position;
	vec3 center = inPosition.xyz;
	float radius = inPosition.w;

	float u0 = gl_TessCoord.y;
	float v0 = gl_TessCoord.x;

	bool h = (u0<0.5);
	float u = h ? u0*2 :   v0;
	float v = h ? v0   : 2*u0-1;
	
	float phi = pi * ( u - 0.5 );
	float theta = 2 * pi * ( v - 0.5 );
	float cosphi = cos(phi);
	vec3 posUnit = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );

	teNormal = normalize(posUnit);
	teColor = (u0<0.5) ? vec4(1,0,0,1) : vec4(0,0,1,1);
	vec3 pos = posUnit * radius + center;
	gl_Position = projectionMatrix*viewMatrix*modelMatrix* vec4(pos, 1);
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
