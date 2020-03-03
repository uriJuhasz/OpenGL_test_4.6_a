#version 400

in vec3 position;
in vec3 normal;
in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragmentPosition;
out vec3 fragmentNormal;
out vec2 fragmentUVCoord;

void main() 
{
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
  fragmentPosition = pos4.xyz;
  fragmentNormal = (modelMatrix*vec4(normal,0)).xyz;
  fragmentUVCoord = uvCoord;
}