#pragma once

#include <string>

#include "Resources.h"
#include "Model.h"

namespace aoe {

class IModelManager {
public:
	virtual ~IModelManager() = default;
	virtual ModelId Load(const std::string& path) = 0;
	virtual ModelId Upload(Model model) = 0;
	virtual ModelId GetDefault() = 0;
	virtual const Model& GetModel(ModelId model_id) = 0;
};

} // namespace aoe