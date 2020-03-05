#ifdef COMPILING_VS

in vec3 position;

void main( )
{
	gl_Position = vec4(position,1);
}

#endif

#ifdef COMPILING_TCS

layout( vertices = 16 ) out;

void main( )
{
	gl_out[ gl_InvocationID ].gl_Position = gl_in[ gl_InvocationID ].gl_Position;

	gl_TessLevelOuter[0] = 16;
	gl_TessLevelOuter[1] = 16;
	gl_TessLevelOuter[2] = 16;
	gl_TessLevelOuter[3] = 16;
	gl_TessLevelInner[0] = 16;
	gl_TessLevelInner[1] = 16;
}
#endif

#ifdef COMPILING_TES

layout( quads, equal_spacing, ccw) in;

out vec3 teNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


vec4 makeVecT0(float u)
{
	return vec4( (1-u) * (1-u) * (1-u), 3 * u * (1-u) * (1-u), 3 * u * u * (1-u),  u * u * u);
}
vec4 makeVecT1(float u)
{
	return 3 * vec4( -(1-u) * (1-u), (1-u) * (1-3*u), u * (2-3*u), u * u);
}

void main( )
{
	mat4x4 matrix = mat4x4(
		gl_in[ 0].gl_Position, gl_in[ 1].gl_Position, gl_in[ 2].gl_Position,gl_in[ 3].gl_Position,
		gl_in[ 4].gl_Position, gl_in[ 5].gl_Position, gl_in[ 6].gl_Position,gl_in[ 7].gl_Position,
		gl_in[ 8].gl_Position, gl_in[ 9].gl_Position, gl_in[10].gl_Position,gl_in[11].gl_Position,
		gl_in[12].gl_Position, gl_in[13].gl_Position, gl_in[14].gl_Position,gl_in[15].gl_Position);
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// the basis functions:
	vec4 bu = makeVecT0(u);
	float dbu = makeVecT1(u);
	vec4 bv = makeVecT0(v);
	float dbv = makeVecT1(v);

	// finally, we get to compute something:
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * bu * (matrix * bv);
/*	  bu0 * ( bv0*p00 + bv1*p01 + bv2*p02 + bv3*p03 )
	+ bu1 * ( bv0*p10 + bv1*p11 + bv2*p12 + bv3*p13 )
	+ bu2 * ( bv0*p20 + bv1*p21 + bv2*p22 + bv3*p23 )
	+ bu3 * ( bv0*p30 + bv1*p31 + bv2*p32 + bv3*p33 );*/

	vec4 dpdu = dbu * (matrix*bv);

	vec4 dpdv = bu * (matrix*dbv); 
/*	  bu0 * ( dbv0*p00 + dbv1*p01 + dbv2*p02 + dbv3*p03 )
	+ bu1 * ( dbv0*p10 + dbv1*p11 + dbv2*p12 + dbv3*p13 )
	+ bu2 * ( dbv0*p20 + dbv1*p21 + dbv2*p22 + dbv3*p23 )
	+ bu3 * ( dbv0*p30 + dbv1*p31 + dbv2*p32 + dbv3*p33 );*/

	teNormal = normalize( cross( dpdu.xyz, dpdv.xyz ) );
}

#endif

#ifdef COMPILING_GS

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in   vec4 tessVertex;
out  vec4 geometryVertex;
void main() 
{
    for (int i=0; i<3; ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
}
#endif

#ifdef COMPILING_FS

uniform vec3 edgeColor;

out vec4 frag_color;

void main() 
{
  frag_color = vec4(edgeColor,1);
}
#endif
