#pragma once

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "ModelLoaderOptions.h"

namespace aoe {

class ModelLoader {
public:
	ModelLoader() = delete;

	static Model Load(const std::wstring& path, ModelLoaderOptions options);

private:
	static void ProcessNode(std::vector<Mesh>& meshes, const aiScene* scene, const aiNode* node);
	static Mesh ProcessMesh(const aiScene* scene, const aiMesh* mesh);

	static Vector2f ToVector2(aiVector3D vector);
	static Vector3f ToVector3(aiVector3D vector);
};

} // namespace aoe