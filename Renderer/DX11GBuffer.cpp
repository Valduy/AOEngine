#include "pch.h"

#include "../Core/Debug.h"


#include "DX11GBuffer.h"

namespace aoe {

DX11GBuffer::DX11GBuffer(uint32_t width, uint32_t height)
	: diffuse_(GetTexture8x4Description(width, height))
	, specular_(GetTexture32x4Description(width, height))
	, normal_(GetTexture32x4Description(width, height))
	, position_(GetTexture32x4Description(width, height))
	, accumulator_(GetTexture32x4Description(width, height))
	, width_(0)
	, height_(0)
{
	TryResize(width, height);
}

const DX11GPUTexture2D* DX11GBuffer::GetDiffuseTexture() {
	return diffuse_.GetTexture();
}

const DX11GPUTexture2D* DX11GBuffer::GetSpecularTexture() {
	return specular_.GetTexture();
}

const DX11GPUTexture2D* DX11GBuffer::GetNormalTexture() {
	return normal_.GetTexture();
}

const DX11GPUTexture2D* DX11GBuffer::GetPositionTexture() {
	return position_.GetTexture();
}

const DX11GPUTexture2D* DX11GBuffer::GetAccumulatorTexture() {
	return accumulator_.GetTexture();
}

bool DX11GBuffer::TryResize(uint32_t width, uint32_t height) {
	AOE_ASSERT_MSG(width > 0, "Width equal to zero.");
	AOE_ASSERT_MSG(height > 0, "Height equal to zero.");

	if (width_ == width && height_ == height) {
		return false;
	}

	width_ = width;
	height_ = height;

	diffuse_.TryResize(width, height);
	specular_.TryResize(width, height);
	normal_.TryResize(width, height);
	position_.TryResize(width, height);
	accumulator_.TryResize(width, height);

	return true;
}

GPUTexture2DDescription DX11GBuffer::GetTexture8x4Description(uint32_t width, uint32_t height) {
	return {
		width,
		height,
		GPUPixelFormat::kR8G8B8A8_Unorm,
		GPUTextureFlags::kRenderTarget | GPUTextureFlags::kShaderResource
	};
}

GPUTexture2DDescription DX11GBuffer::GetTexture32x4Description(uint32_t width, uint32_t height) {
	return {
		width,
		height,
		GPUPixelFormat::kR32G32B32A32_Float,
		GPUTextureFlags::kRenderTarget | GPUTextureFlags::kShaderResource
	};
}

} // namespace aoe