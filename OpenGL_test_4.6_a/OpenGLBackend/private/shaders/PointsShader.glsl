uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

#ifdef COMPILING_VS

in vec3 position;

void main() 
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}

#endif

#ifdef COMPILING_FS

uniform vec4 pointColor = vec4(1,0,0,1);

out vec4 frag_color;

void main() {
	frag_color = pointColor;
}

#endif
