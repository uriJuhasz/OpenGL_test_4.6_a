struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};


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

out VertexData teVertexData;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


vec4 makeVecT0(float u)
{
	return vec4( (1-u) * (1-u) * (1-u), 3 * u * (1-u) * (1-u), 3 * u * u * (1-u),  u * u * u);
}
vec4 makeVecT1(float u)
{
	return 3 * vec4( -(1-u) * (1-u), (1-u) * (1-3*u), u * (2-3*u), u * u);
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
	vec4 dbu = makeVecT1(u);
	vec4 bv = makeVecT0(v);
	vec4 dbv = makeVecT1(v);

	vec4 position = someOp(P, bu,bv);

	vec4 dpdu = someOp(P, dbu, bv);
	vec4 dpdv = someOp(P, bu, dbv);

	vec4 worldPosition = modelMatrix*position;
	gl_Position = (projectionMatrix * viewMatrix) * worldPosition;

	teVertexData = VertexData(
	    worldPosition.xyz,
		(modelMatrix*vec4(normalize(cross( dpdu.xyz, dpdv.xyz )),0)).xyz,
		vec2(u,v)
	);

}

#endif

#ifdef COMPILING_GS

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location=1) in VertexData teVertexData[];
layout (location=1) out VertexData gVertexData;

void main() 
{
    for (int i=0; i<3; ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		gVertexData = teVertexData[i];
		EmitVertex();
	}
}
#endif

#ifdef COMPILING_FS

uniform vec3 viewerPosition;

uniform vec3 frontColor;
uniform vec3 backColor;

uniform vec3 light0Position;
uniform vec3 light0Color;
uniform float light0SpecularExponent;
	
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform float light1SpecularExponent;

layout (location=1) in VertexData gVertexData;

out vec4 fragmentColor;

vec3 calculateLight(vec3 lightPosition, vec3 lightColor, float lightSpecularExponent, vec3 baseColor, vec3 normal)
{
	vec3 lightDirection = normalize(lightPosition-gVertexData.position);
	float diffuseLight = dot(lightDirection,normal);
	if (diffuseLight<0)
		return vec3(0,0,0);
	vec3 diffuseColor = clamp(diffuseLight * baseColor,0,1);

	vec3 viewerDirection = normalize(viewerPosition-gVertexData.position);
	vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;

	float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),lightSpecularExponent);
	vec3 specularColor = specularIntensity*lightColor;

	return clamp(diffuseColor + specularColor,0,1);
}
vec3 calculateBaseColor(vec2 uvCoord)
{
	return
		vec3(1,0,0)*(clamp(1-uvCoord.x-uvCoord.y,0,1) + clamp(uvCoord.x-uvCoord.y,0,1)) +
		vec3(0,1,0)*(clamp(uvCoord.y-uvCoord.x,0,1) + clamp(uvCoord.x-uvCoord.y,0,1)) +
		vec3(0,0,1)*(clamp(uvCoord.x+uvCoord.y-1.0f,0,1))
	;
}
void main() 
{
	vec2 uvCoord = gVertexData.uvCoord;
	float radius = length(uvCoord-vec2(0.5,0.5));
	if (radius<0.2)
		discard;

	vec3 normal = normalize(gVertexData.normal) *(gl_FrontFacing ? 1 : -1);

	vec3 color = gl_FrontFacing ? calculateBaseColor(uvCoord) : vec3(1,1,1)-calculateBaseColor(uvCoord);//backColor;

	vec3 light0Component = calculateLight(light0Position,light0Color,light0SpecularExponent,color, normal);
	vec3 light1Component = calculateLight(light1Position,light1Color,light1SpecularExponent,color, normal);
	fragmentColor = vec4(light0Component + light1Component ,1);
}
#endif
