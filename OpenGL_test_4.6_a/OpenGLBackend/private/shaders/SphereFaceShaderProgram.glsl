uniform mat4 modelMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 viewMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
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
//out vec2 teLocalUV;

uniform int drawEdges = 1;

const float pi = radians(180);

void main( )
{
	vec4 inPosition = gl_in[0].gl_Position;
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
	teVertexData = VertexData(
	    worldPosition.xyz,
		(modelMatrix*vec4(normal,0)).xyz,
		vec2(u,v)
	);
/*	if (drawEdges==1)
	{
		const float tessellationLevel = gl_TessLevelInner[0];
		teLocalUV = vec2(mod(int(u*tessellationLevel),2),mod(int(v*tessellationLevel),2));
	}*/

}

#endif

#ifdef COMPILING_FS

layout (location=1) in VertexData teVertexData;
//in vec2 teLocalUV;

out vec4 fragmentColor;

uniform vec3 viewerPosition = vec3(0,0,-10);

uniform vec3 frontColor = vec3(1,0,0);
uniform vec3 backColor = vec3(0,0,1);

uniform float ambientLightIntensity = 0.3;

uniform vec3 light0Position = vec3(0,100,0);
uniform vec3 light0Color = vec3(1,1,1);
uniform float light0SpecularExponent =  100;
	
uniform vec3 light1Position = vec3(0,-100,0);
uniform vec3 light1Color = vec3(1,1,1);
uniform float light1SpecularExponent = 100;

vec3 calculateLight(vec3 lightPosition, vec3 lightColor, float lightSpecularExponent, vec3 baseColor, vec3 normal)
{
	vec3 color = vec3(0,0,0);

	vec3 lightDirection = normalize(lightPosition-teVertexData.position);
	float diffuseLight = dot(lightDirection,normal);
	if (diffuseLight>=0)
	{
		vec3 diffuseColor = clamp(diffuseLight * baseColor,0,1);

		vec3 viewerDirection = normalize(viewerPosition-teVertexData.position);
		vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;

		float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),lightSpecularExponent);
		vec3 specularColor = specularIntensity*lightColor;

		color += diffuseColor + specularColor;
	}
	return clamp(color,0,1);
}

float line2Triangle(float f)
{
	return f<0.5 ? f*2 : 1-(f-0.5)*2;
}

vec3 calculateBaseColor(vec2 uvCoordX)
{
	vec2 uvCoord = vec2( uvCoordX[0], line2Triangle(uvCoordX[1]) );
	float u = uvCoord[0];
	float v = uvCoord[1];
	return
		vec3(1,0,0)*(clamp(1-u-v,0,1) + clamp(u-v,0,1)) +
		vec3(0,1,0)*(clamp(v-u,0,1) + clamp(u-v,0,1)) +
		vec3(0,0,1)*(clamp(u+v-1.0f,0,1))
	;
}

/*\uniform int drawEdges = 1;
uniform vec4 edgeColor = vec4(1,1,1,1);

const float eps = 0.1;
bool isQuadEdgeCoord(float x)
{
	return abs(x-0)<eps || abs(1-x)<eps;
}
bool isQuadEdge(vec2 uv)
{
	return isQuadEdgeCoord(uv[0]) || isQuadEdgeCoord(uv[1]);
}
*/
void main() {
	vec2 uvCoord = teVertexData.uvCoord;

/*	if ((drawEdges==1) && isQuadEdge(teLocalUV))
	{
		fragmentColor = edgeColor;
	}
	else*/
	{
		vec3 normal = normalize(teVertexData.normal);
		vec2 vuCoord = vec2(uvCoord[1],uvCoord[0]);
		vec3 baseColor =  calculateBaseColor(uvCoord);
		
		vec3 ambientLightComponent = ambientLightIntensity * baseColor;

		vec3 light0Component = calculateLight(light0Position,light0Color,light0SpecularExponent,baseColor, normal);
		vec3 light1Component = calculateLight(light1Position,light1Color,light1SpecularExponent,baseColor, normal);
		fragmentColor = vec4(ambientLightComponent+light0Component + light1Component ,1);
	}
}

#endif
