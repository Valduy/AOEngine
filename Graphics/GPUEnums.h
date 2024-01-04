#pragma once

#include "../Core/EnumHelper.h"

namespace aoe {

enum class GPUTextureFlags {
    kNone = 0x0000,
    kShaderResource = 0x0001,
    kRenderTarget = 0x0002,
    kDepthStencil = 0x0004,
    kUnorderedAccess = 0x0008,
};

enum class GPUDepthWriteMask {
    kWriteNone = 0,
    kWriteAll = 1,
};

enum class GPUComparsionFunc
{
    kNever = 1,
    kLess = 2,
    kEqual = 3,
    kLessOrEqual = 4,
    kGreater = 5,
    kNotEqual = 6,
    kGreaterOrEqual = 7,
    kAlways = 8
};

enum class GPUCullMode {
    kNone = 1,
    kFront = 2,
    kBack = 3,
};

enum class GPUFillMode {
    kWireframe = 2,
    kSolid = 3,
};

enum class GPUResourceUsage {
    kDefault = 0,
    kDynamic = 2,
};

enum class LayoutElementSemantics {
    kBinormal,
    kBlendIndices,
    kBlendWeight,
    kColor,
    kNormal,
    kPosition,
    kPositionT,
    kPSize,
    kTangent,
    kTexcoord,
};

enum class GPUPrimitiveTopology {
    kPointList = 1,
    kLineList = 2,
    kLineStrip = 3,
    kTriangleList = 4,
    kTriangleStrip = 5,
};

enum class GPUShaderType {
    kVertex,
    kPixel,
};

enum class GPUSamplerFilter {
    kPoint,
    kBilinear,
    kTrilinear,
    kAnisotropic,
};

enum class GPUSamplerComparsionFunction {
    kNever,
    kLess,
};

enum class GPUSamplerAddressMode {
    kWrap,
    kClamp,
    kMirror,
    kBorder,
};

enum class GPUSamplerBorderColor {
    kTransparentBlack,
    kOpaqueBlack,
    kOpaqueWhite,
};

} // namespace aoe

AOE_DECLARE_ENUM_OPERATORS(aoe::GPUTextureFlags)