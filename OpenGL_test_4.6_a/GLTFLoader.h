#ifndef GLTFLoader_H_
#define GLTFLoader_H_

#include "Mesh.h"

#include <string>
#include <memory>

class GLTFLoader
{
	static std::unique_ptr<Mesh> loadGLTF(const std::string& fileName);
};

#endif //GLTFLoader_H_