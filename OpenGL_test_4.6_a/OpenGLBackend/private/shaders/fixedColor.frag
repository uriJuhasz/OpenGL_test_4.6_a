#version 460 core

uniform vec4 fixedFragmentColor = vec4(1,1,1,1);

out vec4 fragmentColor;

void main() {
	fragmentColor = fixedFragmentColor;
}
