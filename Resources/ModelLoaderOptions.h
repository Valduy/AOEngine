#pragma once

#include "../Core/EnumHelper.h"

namespace aoe {

enum class ModelLoaderOptions {
    kCalculateTangentSpace = 0x1,
    kJoinIdenticalVertices = 0x2,
    kMakeLeftHanded = 0x4,
    kTriangulate = 0x8,
    kRemoveComponent = 0x10,
    kGenerateNormals = 0x20,
    kGenerateSmoothNormals = 0x40,
    kSplitLargeMeshes = 0x80,
    kPreTransformVertices = 0x100,
    kLimitBoneWeights = 0x200,
    kValidateDataStructure = 0x400,
    kImproveCacheLocality = 0x800,
    kRemoveRedundantMaterials = 0x1000,
    kFixInfacingNormals = 0x2000,
    kPopulateArmatureData = 0x4000,
    kSortByPType = 0x8000,
    kFindDegenerates = 0x10000,
    kFindInvalidData = 0x20000,
    kGenerateUVCoords = 0x40000,
    kTransformUVCoords = 0x80000,
    kFindInstances = 0x100000,
    kOptimizeMeshes = 0x200000,
    kOptimizeGraph = 0x400000,
    kFlipUVs = 0x800000,
    kFlipWindingOrder = 0x1000000,
    kSplitByBoneCount = 0x2000000,
    kDebone = 0x4000000,
    kGlobalScale = 0x8000000,
    kEmbedTextures = 0x10000000,
    kForceGenerateNormals = 0x20000000,
    kDropNormals = 0x40000000,
    kGenerateBoundingBoxes = 0x80000000,

    kConvertToLeftHanded = kMakeLeftHanded | kFlipUVs | kFlipWindingOrder,

    kPresetRealtimeFast = kCalculateTangentSpace |
    kGenerateNormals |
    kJoinIdenticalVertices |
    kTriangulate |
    kGenerateUVCoords |
    kSortByPType,

    kPresetRealtimeQuality = kCalculateTangentSpace |
    kGenerateSmoothNormals |
    kJoinIdenticalVertices |
    kImproveCacheLocality |
    kLimitBoneWeights |
    kRemoveRedundantMaterials |
    kSplitLargeMeshes |
    kTriangulate |
    kGenerateUVCoords |
    kSortByPType |
    kFindDegenerates |
    kFindInvalidData,

    kPresetRealtimeMaxQuality = kPresetRealtimeQuality | kFindInstances | kValidateDataStructure | kOptimizeMeshes
};

} // namespace aoe

AOE_DECLARE_ENUM_OPERATORS(aoe::ModelLoaderOptions)