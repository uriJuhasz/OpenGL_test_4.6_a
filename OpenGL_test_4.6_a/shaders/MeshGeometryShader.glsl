#version 430

struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};

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
//    vec4 mid = (p0+p1+p2)/3 + vec4(normal,0)*10.0f;

    VertexData ovs[3] = { originalVertex[0], originalVertex[1], originalVertex[2] };

    VertexData realMid = VertexData(
        (ovs[0].position+ovs[1].position+ovs[2].position)/3,
        normalize(ovs[0].normal+ovs[1].normal+ovs[2].normal),
        (ovs[0].uvCoord+ovs[1].uvCoord+ovs[2].uvCoord)/3
    );

    VertexData midVertex = realMid;
    midVertex.position += normal*1.0f;

    VertexData nvs[3] = {
        mixVD(ovs[0],realMid,0.5),
        mixVD(ovs[1],realMid,0.5),
        mixVD(ovs[2],realMid,0.5),
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

/*
    emitVertexData(nvs[0]);
    emitVertexData(nvs[1]);
    emitVertexData(midVertex);
    emitVertexData(nvs[2]);
    emitVertexData(nvs[0]);
    emitVertexData(ovs[0]);
    emitVertexData(nvs[1]);
    emitVertexData(ovs[1]);
    emitVertexData(nvs[2]);
    emitVertexData(ovs[0]);
    emitVertexData(nvs[0]);
    */
}
