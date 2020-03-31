struct VertexData
{
    vec2 uvCoord;
};

#ifdef COMPILING_VS

in vec3 position;
in vec2 uvCoord;

out VertexData vsVertexData;

void main() 
{
    vec4 worldPos = vec4(position, 1.0);
    gl_Position = worldPos;
    vsVertexData = VertexData(uvCoord);
}

#endif

#ifdef COMPILING_FS
in VertexData vsVertexData;

out vec4 fragmentColor;

uniform sampler2D mainTexture;

void main() {
    const float u = vsVertexData.uvCoord[0];
    const float v = vsVertexData.uvCoord[1];
	fragmentColor = 
        texture(mainTexture, vsVertexData.uvCoord); 
        //vec4(u,v,1-u-v,1);
}

#endif
