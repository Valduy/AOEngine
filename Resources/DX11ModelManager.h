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
	DX11ModelManager();

	virtual ModelId Load(const std::string& path) override;
	virtual ModelId Upload(Model model) override;
	const Model& GetModel(ModelId model_id) override;
	const DX11ModelResources& GetMeshResources(ModelId mesh_id);

private:
	std::unordered_map<std::string, ModelId> path_to_model_id_;

	std::vector<Model> models_;
	std::vector<DX11ModelResources> models_resources_;

	DX11ModelResources CreateModelResources(const Model& model);
	DX11GPUBuffer CreateVertexBuffer(const Mesh& mesh);
	DX11GPUBuffer CreateIndexBuffer(const Mesh& mesh);
};

} // namespace aoe