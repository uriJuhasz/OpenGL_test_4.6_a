#version 410

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

struct VertexData
{
    vec3 position;
    vec3 normal;
    vec2 uvCoord;
};
layout (location=1) out VertexData originalVertex;
void main() 
{
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    gl_Position = worldPos;
    originalVertex = VertexData(
    worldPos.xyz, (modelMatrix*vec4(normal,0)).xyz, uvCoord
    );
}

/*
void main() 
{
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
  originalVertex.position = pos4.xyz;
  originalVertex.normal   = (modelMatrix*vec4(normal,0)).xyz;
  originalVertex.uvCoord  = uvCoord;
}
*/