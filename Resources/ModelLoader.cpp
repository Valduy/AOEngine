#include "pch.h"

#include "../Core/FileHelper.h"
#include "../Application/Platform.h"

#include "ModelLoader.h"

namespace aoe {

Model ModelLoader::Load(const std::wstring& path, ModelLoaderOptions options) {
	std::wstring full_path = std::format(L"{}/{}", Platform::GetExecutableDirectory(), path);
	std::vector<char> buffer = FileHelper::ReadAllFile(full_path);
	std::string extension = FileHelper::GetExtension(full_path, ExtensionOption::kWithoutDot);
	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(
		buffer.data(), 
		buffer.size(), 
		static_cast<uint32_t>(options), 
		extension.c_str());
	
	if (scene == nullptr) {
		throw std::runtime_error("Exception during model loading: scene == nullptr.");
	}

	if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		throw std::runtime_error("Exception during model loading: scene is incomplete.");
	}

	if (scene->mRootNode == nullptr) {
		throw std::runtime_error("Exception during model loading: there is no scene root.");
	}

	std::vector<Mesh> meshes;
	ProcessNode(meshes, scene, scene->mRootNode);

	return { meshes };
}

void ModelLoader::ProcessNode(std::vector<Mesh>& meshes, const aiScene* scene, const aiNode* node) {
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(scene, mesh));
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(meshes, scene, node->mChildren[i]);
	}
}

Mesh ModelLoader::ProcessMesh(const aiScene* scene, const aiMesh* mesh) {
	std::vector<Vertex> vertices;
	std::vector<Index> indices;

	for (size_t i = 0; i < mesh->mNumVertices; ++i) {
		const Vector3 position = ToVector3(mesh->mVertices[i]);

		Vector3 normal = Math::kV3Zero;
		Vector2 uv = Math::kV2Zero;

		if (mesh->HasNormals()) {
			normal = ToVector3(mesh->mNormals[i]);
		}

		if (mesh->mTextureCoords[0] != nullptr) {
			uv = ToVector2(mesh->mTextureCoords[0][i]);
		}

		vertices.push_back({ position, normal, uv });
	}

	for (size_t i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace face = mesh->mFaces[i];

		for (size_t j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	return { vertices, indices };
}

Vector2 ModelLoader::ToVector2(aiVector3D vector) {
	return { vector.x, vector.y };
}

Vector3 ModelLoader::ToVector3(aiVector3D vector) {
	return { vector.x, vector.y, vector.z };
}

} // namespace aoe