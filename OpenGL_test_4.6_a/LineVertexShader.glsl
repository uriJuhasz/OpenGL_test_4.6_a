#version 400

in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
}