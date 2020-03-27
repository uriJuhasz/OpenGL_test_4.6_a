uniform mat4 modelMatrix      = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 viewMatrix       = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 projectionMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};

#ifdef COMPILING_VS

layout (location = 0) in vec4 centerRadius;

void main( )
{
	gl_Position = centerRadius;
}

#endif

#ifdef COMPILING_TCS

layout( vertices = 1 ) out;

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
	
	float tessellationLevel = calculateTessellationLevel(center, radius);

	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
	gl_TessLevelOuter[2] = tessellationLevel;
	gl_TessLevelOuter[3] = tessellationLevel;
	gl_TessLevelInner[0] = tessellationLevel;
	gl_TessLevelInner[1] = tessellationLevel;
}


#endif

#ifdef COMPILING_TES

layout( quads, equal_spacing, ccw) in;


layout (location=1) out VertexData teVertexData;

const float pi = radians(180);

void main( )
{
	const float tessellationLevel = gl_TessLevelInner[0];

	const vec4 inPosition = gl_in[0].gl_Position;
	vec3 center = inPosition.xyz;
	float radius = inPosition.w;

	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	float phi   = pi * (u - 0.5);
	float theta = 2 * pi * v;
	float cosphi = cos(phi);
	vec3 posUnit = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );

	vec3 normal = normalize(posUnit);
	vec3 pos = posUnit * radius + center;
	vec4 worldPosition = modelMatrix* vec4(pos, 1);
	gl_Position = projectionMatrix*viewMatrix*worldPosition;

	vec2 cbUV = vec2(mod(int(u*tessellationLevel),2),mod(int(v*tessellationLevel),2));

	teVertexData = VertexData(
	    worldPosition.xyz,
		(modelMatrix*vec4(normal,0)).xyz,
		cbUV //vec2(u,v)
	);
}

#endif

#ifdef COMPILING_FS

layout (location=1) in VertexData teVertexData;

uniform vec4 edgeColor = vec4(1,1,1,1); 

out vec4 fragmentColor;


const float eps = 0.0001;
bool isQuadEdgeCoord(float x)
{
	return abs(x-0)<eps || abs(1-x)<eps;
}
bool isQuadEdge(vec2 uvCoord)
{
	return isQuadEdgeCoord(uvCoord[0]) || isQuadEdgeCoord(uvCoord[1]);
}
void main() {
	vec2 uvCoord = teVertexData.uvCoord;
	if (!isQuadEdge(uvCoord))
		discard;

	fragmentColor = edgeColor;
}

#endif
