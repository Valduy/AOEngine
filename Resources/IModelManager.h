#pragma once

#include <string>

#include "ModelLoader.h"
#include "Resources.h"

namespace aoe {

class IModelManager {
public:
	virtual ~IModelManager() = default;
	virtual ModelId Load(const std::wstring& path, ModelLoaderOptions options) = 0;
	virtual ModelId Upload(Model model) = 0;
	virtual ModelId GetDefault() = 0;
	virtual const Model& GetModel(ModelId model_id) = 0;
};

} // namespace aoe