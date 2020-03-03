#include "GLTFLoader.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using namespace std;

#define CGLTF_IMPLEMENTATION
#include "/Code/yocto-gl/yocto/ext/cgltf.h"

std::unique_ptr<Mesh> GLTFLoader::loadGLTF(const std::string& fileName)
{
	cgltf_data* dataPtr = nullptr;
	const cgltf_options options;
	cgltf_parse_file(&options, fileName.c_str(), &dataPtr);
	cgltf_free(dataPtr);
/*
	vector<char> data;
	{
		ifstream file(fileName, std::ios::basic_ios::binary | std::ios::basic_ios::in);
		if (!file.is_open)
		{
			return std::unique_ptr<Mesh>();
		}

		const auto fileSize = fs::file_size(fileName);
		if (fileSize <= 0)
		{
			return std::unique_ptr<Mesh>();
		}

		data.resize(fileSize);
		file.read(data.data(), fileSize);
		if (!file)
		{
			return std::unique_ptr<Mesh>();
		}
	}
	*/

}
