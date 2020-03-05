#version 400 compatibility

out vec3 vCenter;
out float vRadius;

void main( )
{
	vCenter = gl_Vertex.xyz;
	vRadius = gl_Vertex.w;
	gl_Position = vec4( 0., 0., 0., 1. ); // doesn’t matter now – we will in the cords later
}
