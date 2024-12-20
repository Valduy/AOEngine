#include "pch.h"

#include "../Application/Platform.h"

#include "DX11ModelManager.h"

namespace aoe {

DX11ModelManager::DX11ModelManager()
	: path_to_model_id_()
	, models_()
	, models_resources_()
{
	Upload(Model());
}

ModelId DX11ModelManager::Load(const std::wstring& path, ModelLoaderOptions options) {
	auto it = path_to_model_id_.find(path);

	if (it != path_to_model_id_.end()) {
		return it->second;
	}

	ModelId id = Upload(ModelLoader::Load(path, options));
	path_to_model_id_[path] = id;
	return id;
}

ModelId DX11ModelManager::Upload(Model model) {
	models_.emplace_back(std::move(model));
	models_resources_.emplace_back(CreateModelResources(models_.back()));
	return static_cast<ModelId>(models_.size() - 1);
}

ModelId DX11ModelManager::GetDefault() {
	return kDefault;
}

const Model& DX11ModelManager::GetModel(ModelId model_id) {
	return models_[model_id];
}

const DX11ModelResources& DX11ModelManager::GetModelResources(ModelId model_id) {
	return models_resources_[model_id];
}

DX11ModelResources DX11ModelManager::CreateModelResources(const Model& model) {
	std::vector<DX11MeshResources> meshes_resources;
	meshes_resources.reserve(model.GetMeshes().size());

	for (const Mesh& mesh : model.GetMeshes()) {
		DX11MeshResources mesh_resources{
			CreateVertexBuffer(mesh),
			CreateIndexBuffer(mesh),
		};

		meshes_resources.emplace_back(std::move(mesh_resources));
	}

	DX11ModelResources model_resources{
		std::move(meshes_resources),
	};

	return model_resources;
}

DX11GPUBuffer DX11ModelManager::CreateVertexBuffer(const Mesh& mesh) {
	const GPUBufferDescription vertex_buffer_desc{
		GPUBufferType::kVertexBuffer,
		GPUResourceUsage::kDefault
	};

	const std::vector<Vertex>& vertices = mesh.GetVertices();
	return DX11GPUBuffer::Create<Vertex>(vertex_buffer_desc, vertices.data(), vertices.size());
}

DX11GPUBuffer DX11ModelManager::CreateIndexBuffer(const Mesh& mesh) {
	const GPUBufferDescription index_buffer_desc{
		GPUBufferType::kIndexBuffer,
		GPUResourceUsage::kDefault
	};

	const std::vector<Index>& indices = mesh.GetIndices();
	return DX11GPUBuffer::Create<Index>(index_buffer_desc, indices.data(), indices.size());
}

} // namespace aoe