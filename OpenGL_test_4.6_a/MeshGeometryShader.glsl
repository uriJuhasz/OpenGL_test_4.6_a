#version 410

out struct VertexData
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
layout (triangle_strip, max_vertices = 5) out;


layout (location=1) out VertexData geometryVertex;


void main() 
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec3 areaNormal = -cross((p2-p0).xyz,(p1-p0).xyz);
    vec3 normal = normalize(areaNormal) * 0.3f;//length(areaNormal)*1.04.4;
    vec4 mid = (p0+p1+p2)/3 + vec4(normal,0);
    
    VertexData midVertex = VertexData(
        (originalVertex[0].position+originalVertex[1].position+originalVertex[2].position)/3,
        normalize(areaNormal),//(originalVertex[0].normal+originalVertex[1].normal+originalVertex[2].normal)/3,
        (originalVertex[0].uvCoord+originalVertex[1].uvCoord+originalVertex[2].uvCoord)/3
    );

    gl_Position = projectionMatrix*viewMatrix*p0;
    geometryVertex = originalVertex[0];
    EmitVertex();

    gl_Position = projectionMatrix*viewMatrix*p1;
    geometryVertex = originalVertex[1];
    EmitVertex();

    gl_Position = projectionMatrix*viewMatrix*mid;
    geometryVertex = midVertex;
    EmitVertex();

    gl_Position = projectionMatrix*viewMatrix*p2;
    geometryVertex = originalVertex[2];
    EmitVertex();

    gl_Position = projectionMatrix*viewMatrix*p0;
    geometryVertex = originalVertex[0];
    EmitVertex();
}
