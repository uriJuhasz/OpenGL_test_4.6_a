#ifdef COMPILING_VS

layout (location = 0) in vec3 position;

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

vec4  someOp(vec4 P[4][4], vec4 a, vec4 b)
{
	vec4 r = vec4(0,0,0,0);

	for (int i=0; i<4; ++i)
	{
		for (int j=0; j<4; ++j)
		{
			r += a[i]*P[i][j]*b[j];
		}
	}
	return r;
}
void main( )
{
	vec4 P[4][4] = {
		{gl_in[ 0].gl_Position, gl_in[ 1].gl_Position, gl_in[ 2].gl_Position,gl_in[ 3].gl_Position},
		{gl_in[ 4].gl_Position, gl_in[ 5].gl_Position, gl_in[ 6].gl_Position,gl_in[ 7].gl_Position},
		{gl_in[ 8].gl_Position, gl_in[ 9].gl_Position, gl_in[10].gl_Position,gl_in[11].gl_Position},
		{gl_in[12].gl_Position, gl_in[13].gl_Position, gl_in[14].gl_Position,gl_in[15].gl_Position}
		};
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// the basis functions:
	vec4 bu = makeVecT0(u);
	vec4 dbu = makeVecT1(u);
	vec4 bv = makeVecT0(v);
	vec4 dbv = makeVecT1(v);

	// finally, we get to compute something:
	vec4 position = someOp(P, bu,bv);
//	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * position;


	vec4 dpdu = someOp(P, dbu,dbv);
	vec4 dpdv = someOp(P, bu, dbv);


	teNormal = normalize( cross( dpdu.xyz, dpdv.xyz ) );

//	gl_Position = vec4(u-0.5,v-0.5,0,1); 
//	gl_Position = vec4(vec3(-0.5,-0.5,0)+0.3*vec3((P[0][0] * (1-u)*(1-v) + P[0][3]*u*(1-v) + P[3][0]*(1-u)*v + P[3][3]*u*v).xz,0),1);
//	gl_Position = vec4(vec3(-0.5,-0.5,0)+0.3*position.xzy,1);
	gl_Position = (projectionMatrix * viewMatrix * modelMatrix) * position;
}

#endif

#ifdef COMPILING_GS

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//in   vec4 tessVertex[];
//out  vec4 geometryVertex;
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

in vec4 geometryVertex;

out vec4 frag_color;

void main() 
{
  frag_color = vec4(edgeColor,1);
}
#endif
