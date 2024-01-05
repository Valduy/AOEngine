#include "pch.h"

#include "../Core/Debug.h"

#include "GPUTexture2D.h"
#include "DXHelper.h"

namespace aoe {

const ID3D11Texture2D* GPUTexture2D::GetNative() const {
	return texture_;
}

const GPUTexture2DDescription& GPUTexture2D::GetDescription() const {
	return description_;
}

uint32_t GPUTexture2D::GetWidth() const {
	return description_.height;
}

uint32_t GPUTexture2D::GetHeight() const {
	return description_.width;
}

GPUPixelFormat GPUTexture2D::GetPixelFormat() const {
	return description_.pixel_format;
}

bool GPUTexture2D::IsShaderResource() const {
	return (description_.texture_flags & GPUTextureFlags::kShaderResource) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsDepthStencil() const {
	return (description_.texture_flags & GPUTextureFlags::kDepthStencil) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsRenderTarget() const {
	return (description_.texture_flags & GPUTextureFlags::kRenderTarget) != GPUTextureFlags::kNone;
}

bool GPUTexture2D::IsUnorderedAccess() const {
	return (description_.texture_flags & GPUTextureFlags::kUnorderedAccess) != GPUTextureFlags::kNone;
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
	: device_(device)
	, description_()
	, texture_(nullptr)
	, shader_resource_view_(nullptr)
	, depth_stencil_view_(nullptr)
	, render_target_view_(nullptr)
	, unordered_access_view_(nullptr)
{}

bool GPUTexture2D::Initialize(const GPUTexture2DDescription& description) {
	AOE_ASSERT(texture_ == nullptr);
	AOE_ASSERT(shader_resource_view_ == nullptr);
	AOE_ASSERT(depth_stencil_view_ == nullptr);
	AOE_ASSERT(render_target_view_ == nullptr);
	AOE_ASSERT(unordered_access_view_ == nullptr);
	description_ = description;

	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = description_.width;
	texture_desc.Height = description_.height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXHelper::ToDxgiFormat(description_.pixel_format);
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = ToDXBindFlag(description_.texture_flags);
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	HRESULT hr = S_OK;

	if (description_.data != nullptr) {
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = description_.data;
		subresource_data.SysMemPitch = description_.width * description_.stride;
		subresource_data.SysMemSlicePitch = description_.height * description_.width * description_.stride;

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
	AOE_DX_SAFE_RELEASE(unordered_access_view_);
	AOE_DX_SAFE_RELEASE(render_target_view_);
	AOE_DX_SAFE_RELEASE(depth_stencil_view_);
	AOE_DX_SAFE_RELEASE(shader_resource_view_);
	AOE_DX_SAFE_RELEASE(texture_);
}

uint32_t GPUTexture2D::ToDXBindFlag(GPUTextureFlags value) {
	uint32_t result = 0;

	if ((value & GPUTextureFlags::kShaderResource) != GPUTextureFlags::kNone) {
		result |= D3D11_BIND_SHADER_RESOURCE;
	}
	if ((value & GPUTextureFlags::kRenderTarget) != GPUTextureFlags::kNone) {
		result |= D3D11_BIND_RENDER_TARGET;
	}
	if ((value & GPUTextureFlags::kDepthStencil) != GPUTextureFlags::kNone) {
		result |= D3D11_BIND_DEPTH_STENCIL;
	}
	if ((value & GPUTextureFlags::kUnorderedAccess) != GPUTextureFlags::kNone) {
		result |= D3D11_BIND_UNORDERED_ACCESS;
	}

	return result;
}

} // namespace aoe