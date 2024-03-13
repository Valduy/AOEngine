#pragma once

#include <cstdint>
#include <string>

#include "Model.h"

namespace aoe {

using ModelId = uint32_t;

class IModelManager {
public:
	static constexpr ModelId kEmpty = 0;

	virtual ~IModelManager() = default;
	virtual ModelId Load(const std::string& path) = 0;
	virtual ModelId Upload(Model model) = 0;
	virtual const Model& GetModel(ModelId model_id) = 0;
};

} // namespace aoe