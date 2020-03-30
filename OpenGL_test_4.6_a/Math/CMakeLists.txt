set (targetName "Math")

add_library( "${targetName}" "")

target_include_directories("${targetName}" PUBLIC ${root_SOURCE_DIR})

target_sources ("${targetName}"
	PUBLIC
	Vector.h
	Matrix.h

	PRIVATE
	"Math.cpp"
)
