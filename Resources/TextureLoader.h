#pragma once

#include <string>
#include <stdexcept>

#include "../Core/Debug.h"

#include "Image.h"

namespace aoe {

class TextureLoader {
public:
	TextureLoader() = delete;

	static Image Load(const std::string& path, uint32_t desired_channels = 0);
};

} // namespace aoe