#version 400

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

void main() 
{
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec4 mid = (p0+p1+p2)/3;
    gl_Position = p0;
    EmitVertex();

    gl_Position = p1;
    EmitVertex();

    gl_Position = mid;
    EmitVertex();

    gl_Position = p2;
    EmitVertex();

    gl_Position = p0;
    EmitVertex();
}