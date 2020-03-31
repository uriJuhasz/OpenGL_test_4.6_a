#version 460 core

layout (points) in;

layout (points, max_vertices = 1) out;


void main() 
{
    vec4 position = gl_in[0].gl_Position;
    gl_Position = position;
    EmitVertex();
}
