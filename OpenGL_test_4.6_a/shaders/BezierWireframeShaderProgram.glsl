#ifdef COMPILING_VS

layout (location = 0) in vec3 position;

void main( )
{
	gl_Position = vec4(position,1);
}

#endif

#ifdef COMPILING_TCS

layout( vertices = 16 ) out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform int maxTessellationLevel = 64;

uniform int pixelWidth;

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

	return clamp(pixelDiameter/desiredPixelsPerTriangle,2,maxTessellationLevel);
}

void main()
{
	gl_out[ gl_InvocationID ].gl_Position = gl_in[ gl_InvocationID ].gl_Position;

	if (gl_InvocationID==0)
	{
		vec3 bbMin = gl_in[0].gl_Position.xyz;
		vec3 bbMax = gl_in[0].gl_Position.xyz;
		for (int i=1;i<16;++i)
		{				
			vec3 p = gl_in[i].gl_Position.xyz;
			for (int j=0; j<3; ++j)
			{
				bbMin[j] = min(bbMin[j],p[j]);
				bbMax[j] = max(bbMin[j],p[j]);
			}
		}
	    vec3 bbCenter = (bbMin+bbMax)/2;
	    float bbRadius = length(bbMax-bbMin)/2;

		float tessellationLevel = calculateTessellationLevel(bbCenter, bbRadius);

		gl_TessLevelOuter[0] = tessellationLevel;
		gl_TessLevelOuter[1] = tessellationLevel;
		gl_TessLevelOuter[2] = tessellationLevel;
		gl_TessLevelOuter[3] = tessellationLevel;
		gl_TessLevelInner[0] = tessellationLevel;
		gl_TessLevelInner[1] = tessellationLevel;
	}
}
#endif

#ifdef COMPILING_TES

layout( quads, equal_spacing, ccw) in;

out vec2 teUVCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec4 makeVecT0(float u)
{
	return vec4( (1-u) * (1-u) * (1-u), 3 * u * (1-u) * (1-u), 3 * u * u * (1-u),  u * u * u);
}

vec4  someOp(vec4 P[4][4], vec4 a, vec4 b)
{
	vec4 r = vec4(0,0,0,0);

	for (int i=0; i<4; ++i)
	{
		for (int j=0; j<4; ++j)
		{
			r += a[i]*P[i][j]*b[j];
		}
	}
	return r;
}
void main( )
{
	vec4 P[4][4] = {
		{gl_in[ 0].gl_Position, gl_in[ 1].gl_Position, gl_in[ 2].gl_Position,gl_in[ 3].gl_Position},
		{gl_in[ 4].gl_Position, gl_in[ 5].gl_Position, gl_in[ 6].gl_Position,gl_in[ 7].gl_Position},
		{gl_in[ 8].gl_Position, gl_in[ 9].gl_Position, gl_in[10].gl_Position,gl_in[11].gl_Position},
		{gl_in[12].gl_Position, gl_in[13].gl_Position, gl_in[14].gl_Position,gl_in[15].gl_Position}
		};
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// the basis functions:
	vec4 bu = makeVecT0(u);
	vec4 bv = makeVecT0(v);

	vec4 position = someOp(P, bu,bv);

	vec4 worldPosition = modelMatrix*position;
	teUVCoord = vec2(u,v);
	gl_Position = (projectionMatrix * viewMatrix) * worldPosition;
}

#endif

#ifdef COMPILING_FS

uniform vec4 edgeColor;

in vec2 teUVCoord;

out vec4 fragmentColor;

void main() 
{
	vec2 uvCoord = teUVCoord;
	float radius = length(uvCoord-vec2(0.5,0.5));
	if (radius<0.2)
		discard;
		
	fragmentColor = edgeColor;
}
#endif
