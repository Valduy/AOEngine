#pragma once

#include "Mesh.h"

namespace aoe {

class Model {
public:
	Model()
		: meshes_()
	{}

	Model(std::vector<Mesh> meshes)
		: meshes_(std::move(meshes))
	{}

	const std::vector<Mesh>& GetMeshes() const {
		return meshes_;
	}

private:
	const std::vector<Mesh> meshes_;
};

} // namespace aoe