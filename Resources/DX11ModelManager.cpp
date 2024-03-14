#include "pch.h"

#include "DX11ModelManager.h"

namespace aoe {

DX11ModelManager::DX11ModelManager()
	: path_to_model_id_()
	, models_()
	, models_resources_()
{
	Upload(Model());
}

ModelId DX11ModelManager::Load(const std::string& path) {
	auto it = path_to_model_id_.find(path);

	if (it != path_to_model_id_.end()) {
		return it->second;
	}

	ModelId id = Upload(ModelLoader::Load(path));
	path_to_model_id_[path] = id;
	return id;
}

ModelId DX11ModelManager::Upload(Model model) {
	models_.emplace_back(std::move(model));
	models_resources_.emplace_back(CreateModelResources(models_.back()));
	return static_cast<ModelId>(models_.size() - 1);
}

const Model& DX11ModelManager::GetModel(ModelId model_id) {
	return models_[model_id];
}

const DX11ModelResources& DX11ModelManager::GetMeshResources(ModelId mesh_id) {
	return models_resources_[mesh_id];
}

DX11ModelResources DX11ModelManager::CreateModelResources(const Model& model) {
	std::vector<DX11MeshResources> meshes_resources;

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
	const aoe::GPUBufferDescription vertex_buffer_desc{
		aoe::GPUBufferType::kVertexBuffer,
		aoe::GPUResourceUsage::kDefault
	};

	const std::vector<Vertex>& vertices = mesh.GetVertices();
	return aoe::DX11GPUBuffer::Create<Vertex>(vertex_buffer_desc, vertices.data(), vertices.size());
}

DX11GPUBuffer DX11ModelManager::CreateIndexBuffer(const Mesh& mesh) {
	const aoe::GPUBufferDescription index_buffer_desc{
		aoe::GPUBufferType::kIndexBuffer,
		aoe::GPUResourceUsage::kDefault
	};

	const std::vector<Index>& indices = mesh.GetIndices();
	return aoe::DX11GPUBuffer::Create<Index>(index_buffer_desc, indices.data(), indices.size());
}

} // namespace aoe