target_sources ("${targetName}"
	PRIVATE
	
	Scene.cpp
	Camera.cpp
	PointLight.cpp

	Scene.h
	Camera.h
	PointLight.h
)

add_subdirectory(SceneObjects)

