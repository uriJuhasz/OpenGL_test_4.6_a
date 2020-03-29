struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};

#ifdef COMPILING_VS

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location=1) out VertexData originalVertex;
void main() 
{
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    gl_Position = worldPos;
    originalVertex = VertexData(worldPos.xyz, (modelMatrix*vec4(normal,0)).xyz, uvCoord);
}


#endif

#ifdef COMPILING_GS

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout (location=1) in VertexData originalVertex[];

layout (triangles) in;
layout (triangle_strip, max_vertices = 15) out;

VertexData mixVD(VertexData d1, VertexData d2, float f)
{
    return VertexData(
        mix(d1.position,d2.position,f),
        normalize(mix(d1.normal,d2.normal,f)),
        mix(d1.uvCoord,d2.uvCoord,f)
    );
}

layout (location=1) out VertexData geometryVertex;

void emitVertexData(VertexData d)
{
    gl_Position = projectionMatrix*viewMatrix*vec4(d.position,1);
    geometryVertex = d;
    EmitVertex();}

void main() 
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec3 areaNormal = -cross((p2-p0).xyz,(p1-p0).xyz);
    vec3 normal = normalize(areaNormal);

    VertexData ovs[3] = { originalVertex[0], originalVertex[1], originalVertex[2] };
    for (int i=0; i<3;++i)
        emitVertexData(ovs[i]);
//    emitVertexData(ovs[0]);

/*
    VertexData realMid = VertexData(
        (ovs[0].position+ovs[1].position+ovs[2].position)/3,
        normalize(ovs[0].normal+ovs[1].normal+ovs[2].normal),
        (ovs[0].uvCoord+ovs[1].uvCoord+ovs[2].uvCoord)/3
    );

    VertexData midVertex = realMid;
    midVertex.position += normal*1.0f;

    VertexData nvs[3] = {
        mixVD(ovs[0],realMid,0.8),
        mixVD(ovs[1],realMid,0.8),
        mixVD(ovs[2],realMid,0.8),
    };
    emitVertexData(ovs[0]);
    emitVertexData(ovs[1]);
    emitVertexData(nvs[1]);
    emitVertexData(ovs[2]);
    emitVertexData(nvs[2]);
    emitVertexData(ovs[0]);
    emitVertexData(nvs[0]);
    emitVertexData(nvs[1]);
    emitVertexData(midVertex);
    emitVertexData(nvs[2]);
    emitVertexData(nvs[0]);
    */
}

#endif

#ifdef COMPILING_FS
layout (location=1) in VertexData geometryVertex;

struct PointLight
{
    bool  enabled;
    vec3  position;
    vec3  color;
    float specularExponent;    
};

const int maxLights = 16;
layout(std140) uniform Lights
{
  PointLight pointLights[16];
};

uniform vec3 light0Position;
uniform vec3 light0Color;
uniform float light0SpecularExponent;
	
uniform vec3 light1Position;
uniform vec3 light1Color;
uniform float light1SpecularExponent;

uniform vec3 viewerPosition;

out vec4 frag_color;

vec3 calculateLight(vec3 lightPosition, vec3 lightColor, float lightSpecularExponent, vec3 baseColor, vec3 normal)
{
	vec3 lightDirection = normalize(lightPosition-geometryVertex.position);
	float diffuseLight = dot(lightDirection,normal);
	vec3 diffuseColor = clamp(diffuseLight * baseColor,0,1);

	vec3 viewerDirection = normalize(viewerPosition-geometryVertex.position);
	vec3 reflectedLight = 2*diffuseLight*normal - lightDirection;

	float specularIntensity = pow(clamp(dot(reflectedLight,viewerDirection),0,1),lightSpecularExponent);
	vec3 specularColor = specularIntensity*lightColor;

	return clamp(diffuseColor + specularColor,0,1);
}

vec3 calculateBaseColor()
{
	return
		vec3(1,0,0)*(clamp(1-geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1) + clamp(geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1)) +
		vec3(0,1,0)*(clamp(geometryVertex.uvCoord.y-geometryVertex.uvCoord.x,0,1) + clamp(geometryVertex.uvCoord.x-geometryVertex.uvCoord.y,0,1)) +
		vec3(0,0,1)*(clamp(geometryVertex.uvCoord.x+geometryVertex.uvCoord.y-1.0f,0,1))
	;
}

void main() {
	vec3 normal = normalize(geometryVertex.normal);
	vec3 baseColor = calculateBaseColor();

	vec3 light0Component = calculateLight(light0Position,light0Color,light0SpecularExponent,baseColor, normal);
	vec3 light1Component = calculateLight(light1Position,light1Color,light1SpecularExponent,baseColor, normal);
	frag_color = vec4(light0Component + light1Component ,1);
}

#endif
