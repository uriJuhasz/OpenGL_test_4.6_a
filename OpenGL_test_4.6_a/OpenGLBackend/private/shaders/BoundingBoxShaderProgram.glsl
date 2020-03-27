uniform mat4 viewMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);
uniform mat4 projectionMatrix = mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

#ifdef COMPILING_VS

layout (location = 0) in vec3 position;

void main() 
{
  gl_Position = vec4(position, 1.0);
}

#endif

#ifdef COMPILING_GS

layout (lines) in;
layout (line_strip, max_vertices = 24) out;


void emitVertexData(vec3 position)
{
    gl_Position = projectionMatrix*viewMatrix*vec4(position,1);
    EmitVertex();
}

void main() 
{
    vec3 bb[2] = { gl_in[0].gl_Position.xyz,  gl_in[1].gl_Position.xyz};

    vec3 bvs[8] = {
        vec3(bb[0][0],bb[0][1],bb[0][2]),
        vec3(bb[1][0],bb[0][1],bb[0][2]),
        vec3(bb[1][0],bb[1][1],bb[0][2]),
        vec3(bb[0][0],bb[1][1],bb[0][2]),
        vec3(bb[0][0],bb[0][1],bb[1][2]),
        vec3(bb[1][0],bb[0][1],bb[1][2]),
        vec3(bb[1][0],bb[1][1],bb[1][2]),
        vec3(bb[0][0],bb[1][1],bb[1][2])
    };

    const int bbEdgeStrip[] = {
        0, 1,5,1, 2,6,2, 3,7,3, 0,  4,5,6,7, 4
    };
    for (int vi = 0; vi < bbEdgeStrip.length; ++vi)
    {
        emitVertexData(bvs[bbEdgeStrip[vi]]);
    }
}

#endif

#ifdef COMPILING_FS

uniform vec4 edgeColor;

out vec4 frag_color;

void main() {
  frag_color = edgeColor;
}

#endif
