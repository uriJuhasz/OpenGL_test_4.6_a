#ifdef COMPILING_VS

layout (location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
  gl_Position = projectionMatrix*viewMatrix*modelMatrix * vec4(position, 1.0);
}

#endif

#ifdef COMPILING_GS

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

void emitVertexData(vec4 position)
{
    gl_Position = position;
    EmitVertex();
}

void main() 
{
    vec4 p[3] = { gl_in[0].gl_Position,  gl_in[1].gl_Position, gl_in[2].gl_Position};
    vec3 areaNormal = cross((p[2]-p[0]).xyz,(p[1]-p[0]).xyz);
    vec3 normal = normalize(areaNormal) * length(areaNormal)*4.4;
    vec4 mid = (p[0]+p[1]+p[2])/3 + vec4(normal,0);

    for (int i=0; i<3; ++i)
        emitVertexData(p[i]);
}

#endif

#ifdef COMPILING_FS

uniform vec4 edgeColor;

out vec4 frag_color;

void main() 
{
  frag_color = edgeColor;
}
#endif
