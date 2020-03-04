#version 400 compatibility

in float vRadius[ ];
in vec3 vCenter[ ];

patch out float tcRadius;
patch out vec3 tcCenter;
layout( vertices = 1 ) out;

uniform float uDetail;

void main( )
{
	gl_out[ gl_InvocationID ].gl_Position = gl_in[ 0 ].gl_Position; // (0,0,0,1)
	tcCenter = vCenter[ 0 ];
	tcRadius = vRadius[ 0 ];
	gl_TessLevelOuter[0] = 2.;
	gl_TessLevelOuter[1] = tcRadius * uDetail;
	gl_TessLevelOuter[2] = 2.;
	gl_TessLevelOuter[3] = tcRadius * uDetail;
	gl_TessLevelInner[0] = tcRadius * uDetail;
	gl_TessLevelInner[1] = tcRadius * uDetail;
}