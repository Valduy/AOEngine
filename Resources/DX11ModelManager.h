#pragma once

#include <unordered_map>

#include "../Graphics/DX11GPUBuffer.h"

#include "IModelManager.h"
#include "ModelLoader.h"

namespace aoe {

struct DX11MeshResources {
	const aoe::DX11GPUBuffer vertex_buffer;
	const aoe::DX11GPUBuffer index_buffer;
};

struct DX11ModelResources {
	const std::vector<DX11MeshResources> mesh_resources;
};

class DX11ModelManager : public IModelManager {
public:
	DX11ModelManager()
		: path_to_model_id_()
		, models_()
		, models_resources_()
	{
		Upload(Model());
	}

	virtual ModelId Load(const std::string& path) override {
		auto it = path_to_model_id_.find(path);

		if (it != path_to_model_id_.end()) {
			return it->second;
		}

		ModelId id = Upload(ModelLoader::Load(path));
		path_to_model_id_[path] = id;
		return id;
	}

	virtual ModelId Upload(Model model) override {
		models_.emplace_back(std::move(model));
		models_resources_.emplace_back(CreateModelResources(models_.back()));
		return static_cast<ModelId>(models_.size() - 1);
	}

	const Model& GetModel(ModelId model_id) override {
		return models_[model_id];
	}

	const DX11ModelResources& GetMeshResources(ModelId mesh_id) {
		return models_resources_[mesh_id];
	}

private:
	std::unordered_map<std::string, ModelId> path_to_model_id_;

	std::vector<Model> models_;
	std::vector<DX11ModelResources> models_resources_;

	DX11ModelResources CreateModelResources(const Model& model) {
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

	aoe::DX11GPUBuffer CreateVertexBuffer(const Mesh& mesh) {
		const aoe::GPUBufferDescription vertex_buffer_desc{
			aoe::GPUBufferType::kVertexBuffer,
			aoe::GPUResourceUsage::kDefault
		};

		const std::vector<Vertex>& vertices = mesh.GetVertices();
		return aoe::DX11GPUBuffer::Create<Vertex>(vertex_buffer_desc, vertices.data(), vertices.size());
	}

	aoe::DX11GPUBuffer CreateIndexBuffer(const Mesh& mesh) {
		const aoe::GPUBufferDescription index_buffer_desc{
			aoe::GPUBufferType::kIndexBuffer,
			aoe::GPUResourceUsage::kDefault
		};

		const std::vector<Index>& indices = mesh.GetIndices();
		return aoe::DX11GPUBuffer::Create<Index>(index_buffer_desc, indices.data(), indices.size());
	}
};

} // namespace aoe