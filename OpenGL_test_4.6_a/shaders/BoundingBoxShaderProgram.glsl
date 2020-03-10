#ifdef COMPILING_VS

layout (location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
  vec4 pos4 = modelMatrix * vec4(position, 1.0);
  gl_Position = projectionMatrix*viewMatrix*pos4;
}

#endif

#ifdef COMPILING_FS

uniform vec4 edgeColor;

out vec4 frag_color;

void main() {
  frag_color = edgeColor;
}


#endif
