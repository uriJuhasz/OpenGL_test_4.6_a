#version 460 core

uniform vec4 fixedFragmentColor = vec4(1,1,1,1);
uniform vec4 selectionColor = vec4(0,0,0,1);

out vec4 fragmentColor;
out vec4 fragmentSelectionIndex;

void main() {
	fragmentColor = fixedFragmentColor;
	const float pidf = gl_PrimitiveID;
	const float pidn = pidf / 16.0;
	fragmentSelectionIndex = vec4((gl_PrimitiveID >> 2) /  4.0, (gl_PrimitiveID & 1),((gl_PrimitiveID >> 1)& 1),1);
		//vec4(pidn, 1-pidn,0,1);
}
