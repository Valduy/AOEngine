#pragma once

#include <unordered_map>

#include "../Graphics/DX11GPUBuffer.h"

#include "IModelManager.h"
#include "ModelLoader.h"

namespace aoe {

struct DX11MeshResources {
	const DX11GPUBuffer vertex_buffer;
	const DX11GPUBuffer index_buffer;
};

struct DX11ModelResources {
	const std::vector<DX11MeshResources> meshes_resources;
};

class DX11ModelManager : public IModelManager {
public:
	static constexpr ModelId kDefault = 0;

	DX11ModelManager();

	ModelId Load(const std::wstring& path, ModelLoaderOptions options) override;
	ModelId Upload(Model model) override;
	ModelId GetDefault() override;
	const Model& GetModel(ModelId model_id) override;
	const DX11ModelResources& GetModelResources(ModelId mesh_id);

private:
	std::unordered_map<std::wstring, ModelId> path_to_model_id_;
	std::vector<Model> models_;
	std::vector<DX11ModelResources> models_resources_;

	DX11ModelResources CreateModelResources(const Model& model);
	DX11GPUBuffer CreateVertexBuffer(const Mesh& mesh);
	DX11GPUBuffer CreateIndexBuffer(const Mesh& mesh);
};

} // namespace aoe