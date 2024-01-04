#include "pch.h"

#include "../Core/Debug.h"

#include "GPUTexture2D.h"
#include "DXHelper.h"

namespace aoe {

uint32_t GPUTexture2D::GetWidth() const {
	return width_;
}

uint32_t GPUTexture2D::GetHeight() const {
	return width_;
}

GPUPixelFormat GPUTexture2D::GetPixelFormat() const {
	return pixel_format_;
}

bool GPUTexture2D::IsShaderResource() const {
	return (texture_flags_ & GPUTextureFlags::kShaderResource) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsDepthStencil() const {
	return (texture_flags_ & GPUTextureFlags::kDepthStencil) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsRenderTarget() const {
	return (texture_flags_ & GPUTextureFlags::kRenderTarget) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsUnorderedAccess() const {
	return (texture_flags_ & GPUTextureFlags::kUnorderedAccess) != GPUTextureFlags::kNone;
}

GPUShaderResourceView GPUTexture2D::GetShaderResourceView() const {
	AOE_ASSERT(shader_resource_view_ != nullptr);
	return { shader_resource_view_ };
}

GPUDepthStencilView GPUTexture2D::GetDepthStencilView() const {
	AOE_ASSERT(depth_stencil_view_ != nullptr);
	return { depth_stencil_view_ };
}

GPURenderTargetView GPUTexture2D::GetRenderTargetView() const {
	AOE_ASSERT(render_target_view_ != nullptr);
	return { render_target_view_ };
}

GPUUnorderedAccessView GPUTexture2D::GetUnorderedAccessView() const {
	AOE_ASSERT(unordered_access_view_ != nullptr);
	return { unordered_access_view_ };
}

GPUTexture2D::GPUTexture2D(const GPUDevice& device)
	: width_(0)
	, height_(0)
	, pixel_format_(GPUPixelFormat::kUnknown)
	, texture_flags_(GPUTextureFlags::kNone)
	, texture_(nullptr)
	, shader_resource_view_(nullptr)
	, depth_stencil_view_(nullptr)
	, render_target_view_(nullptr)
	, unordered_access_view_(nullptr)
	, device_(device)
{}

bool GPUTexture2D::Initialize(
	uint32_t width, 
	uint32_t height, 
	GPUPixelFormat pixel_format, 
	GPUTextureFlags texture_flags,
	const IGPUTextureData* texture_data)
{
	AOE_ASSERT(texture_ == nullptr);
	AOE_ASSERT(shader_resource_view_ == nullptr);
	AOE_ASSERT(depth_stencil_view_ == nullptr);
	AOE_ASSERT(render_target_view_ == nullptr);
	AOE_ASSERT(unordered_access_view_ == nullptr);

	width_ = width;
	height_ = height;
	pixel_format_ = pixel_format;
	texture_flags_ = texture_flags;

	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXHelper::ToDxgiFormat(pixel_format);
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = DXHelper::ToBindFlag(texture_flags);
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	HRESULT hr = S_OK;

	if (texture_data != nullptr) {
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = texture_data->GetBufferPointer();
		subresource_data.SysMemPitch = width * texture_data->GetByteWidth();
		subresource_data.SysMemSlicePitch = height * width * texture_data->GetByteWidth();

		hr = device_.GetNative()->CreateTexture2D(&texture_desc, &subresource_data,	&texture_);
	} else {
		hr = device_.GetNative()->CreateTexture2D(&texture_desc, nullptr, &texture_);
	}

	AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

	if (IsShaderResource()) {
		hr = device_.GetNative()->CreateShaderResourceView(texture_, nullptr, &shader_resource_view_);
		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);
	}

	if (IsDepthStencil()) {
		hr = device_.GetNative()->CreateDepthStencilView(texture_, nullptr, &depth_stencil_view_);
		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);
	}

	if (IsRenderTarget()) {
		hr = device_.GetNative()->CreateRenderTargetView(texture_, nullptr, &render_target_view_);
		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);
	}

	if (IsUnorderedAccess()) {
		hr = device_.GetNative()->CreateUnorderedAccessView(texture_, nullptr, &unordered_access_view_);
		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);
	}

	return true;
}

void GPUTexture2D::Terminate() {
	AOE_DX_SAFE_RELEASE(texture_);
	AOE_DX_SAFE_RELEASE(shader_resource_view_);
	AOE_DX_SAFE_RELEASE(depth_stencil_view_);
	AOE_DX_SAFE_RELEASE(render_target_view_);
	AOE_DX_SAFE_RELEASE(unordered_access_view_);
}

} // namespace aoe