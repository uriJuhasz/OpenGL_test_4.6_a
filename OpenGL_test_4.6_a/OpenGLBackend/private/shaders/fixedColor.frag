#version 460 core

uniform vec4 fixedFragmentColor = vec4(1,1,1,1);
uniform vec4 selectionColor = vec4(0,0,0,1);

out vec4 fragmentColor;
out vec4 fragmentSelectionIndex;

void main() {
	fragmentColor = fixedFragmentColor;
	fragmentSelectionIndex = selectionColor;
}
