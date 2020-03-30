
target_sources( "${targetName}"
	PRIVATE

	allTranformations.vert
	BezierEdgeShaderProgram.glsl
	BezierFaceShaderProgram.glsl
	boundingBox.geom
	CMakeLists.txt
	fixedColor.frag
	MeshFaceShaderProgram.glsl
	nopVec3.vert
	SphereEdgeShaderProgram.glsl
	SphereFaceShaderProgram.glsl
	textureOnly.glsl
)


