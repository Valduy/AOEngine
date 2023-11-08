#pragma once

#include <cstdint>

namespace aoe {

using EntityId = std::int32_t;
using Version = std::int32_t;
//using Entity = std::uint64_t;

const EntityId kNullEntity = -1;
const Version kNullVersion = -1;

} // namespace aoe