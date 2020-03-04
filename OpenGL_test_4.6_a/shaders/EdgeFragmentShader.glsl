#version 400

uniform vec3 edgeColor;

out vec4 frag_color;

void main() 
{
  frag_color = vec4(edgeColor,1);
}
