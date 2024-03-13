#include "pch.h"

#include "../Core/Debug.h"

#include "DX11GPUTexture2D.h"
#include "DX11Helper.h"

namespace aoe {

DX11GPUTexture2D::DX11GPUTexture2D(
	const GPUTexture2DDescription& description,
	const void* data,
	uint32_t stride
)
	: description_(description)
	, texture_view_(this)
	, texture_(nullptr)
{
	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = description_.width;
	texture_desc.Height = description_.height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DX11Helper::ToDxgiFormat(description_.pixel_format);
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = ToDXBindFlag(description_.texture_flags);
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	HRESULT hr = S_OK;

	if (data != nullptr) {
		D3D11_SUBRESOURCE_DATA subresource_data = {};
		subresource_data.pSysMem = data;
		subresource_data.SysMemPitch = description_.width * stride;
		subresource_data.SysMemSlicePitch = description_.height * description_.width * stride;

		hr = DX11GPUDevice::Instance()->GetNative()->CreateTexture2D(&texture_desc, &subresource_data, texture_.GetAddressOf());
	}
	else {
		hr = DX11GPUDevice::Instance()->GetNative()->CreateTexture2D(&texture_desc, nullptr, texture_.GetAddressOf());
	}

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create texture.");

	CreateTextureViews();
}

DX11GPUTexture2D::DX11GPUTexture2D(const GPUTexture2DDescription& description)
	: DX11GPUTexture2D(description, nullptr, 0)
{}

DX11GPUTexture2D::DX11GPUTexture2D(ID3D11Texture2D* texture)
	: description_()
	, texture_view_(this)
	, texture_(texture)
{
	D3D11_TEXTURE2D_DESC native_description;
	texture->GetDesc(&native_description);

	description_.width = native_description.Width;
	description_.height = native_description.Height;
	description_.pixel_format = DX11Helper::FromDxgiFormat(native_description.Format);
	description_.texture_flags = FromDXBindFlag(native_description.BindFlags);

	CreateTextureViews();
}

ID3D11Texture2D* DX11GPUTexture2D::GetNative() const {
	return texture_.Get();
}

const GPUTexture2DDescription& DX11GPUTexture2D::GetDescription() const {
	return description_;
}

const DX11GPUTextureView& DX11GPUTexture2D::GetTextureView() const {
	return texture_view_;
}

uint32_t DX11GPUTexture2D::GetWidth() const {
	return description_.height;
}

uint32_t DX11GPUTexture2D::GetHeight() const {
	return description_.width;
}

GPUPixelFormat DX11GPUTexture2D::GetPixelFormat() const {
	return description_.pixel_format;
}

bool DX11GPUTexture2D::IsShaderResource() const {
	return (description_.texture_flags & GPUTextureFlags::kShaderResource) != GPUTextureFlags::kNone;
}

bool DX11GPUTexture2D::IsDepthStencil() const {
	return (description_.texture_flags & GPUTextureFlags::kDepthStencil) != GPUTextureFlags::kNone;
}

bool DX11GPUTexture2D::IsRenderTarget() const {
	return (description_.texture_flags & GPUTextureFlags::kRenderTarget) != GPUTextureFlags::kNone;
}

bool DX11GPUTexture2D::IsUnorderedAccess() const {
	return (description_.texture_flags & GPUTextureFlags::kUnorderedAccess) != GPUTextureFlags::kNone;
}

uint32_t DX11GPUTexture2D::ToDXBindFlag(GPUTextureFlags value) {
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

GPUTextureFlags DX11GPUTexture2D::FromDXBindFlag(uint32_t value) {
	GPUTextureFlags result = GPUTextureFlags::kNone;

	if ((value & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE) {
		result |= GPUTextureFlags::kShaderResource;
	}
	if ((value & D3D11_BIND_RENDER_TARGET) == D3D11_BIND_RENDER_TARGET) {
		result |= GPUTextureFlags::kRenderTarget;
	}
	if ((value & D3D11_BIND_DEPTH_STENCIL) == D3D11_BIND_DEPTH_STENCIL) {
		result |= GPUTextureFlags::kDepthStencil;
	}
	if ((value & D3D11_BIND_UNORDERED_ACCESS) == D3D11_BIND_UNORDERED_ACCESS) {
		result |= GPUTextureFlags::kUnorderedAccess;
	}

	return result;
}

void DX11GPUTexture2D::CreateTextureViews() {
	HRESULT hr = S_OK;

	if (IsShaderResource()) {
		ID3D11ShaderResourceView* shader_resource_view;
		hr = DX11GPUDevice::Instance()->GetNative()->CreateShaderResourceView(texture_.Get(), nullptr, &shader_resource_view);
		AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create shader resource view.");
		texture_view_.Attach(shader_resource_view);
	}

	if (IsDepthStencil()) {
		ID3D11DepthStencilView* depth_stencil_view;
		hr = DX11GPUDevice::Instance()->GetNative()->CreateDepthStencilView(texture_.Get(), nullptr, &depth_stencil_view);
		AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create depth stencil view.");
		texture_view_.Attach(depth_stencil_view);
	}

	if (IsRenderTarget()) {
		ID3D11RenderTargetView* render_target_view;
		hr = DX11GPUDevice::Instance()->GetNative()->CreateRenderTargetView(texture_.Get(), nullptr, &render_target_view);
		AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create render target view.");
		texture_view_.Attach(render_target_view);
	}

	if (IsUnorderedAccess()) {
		ID3D11UnorderedAccessView* unordered_access_view;
		hr = DX11GPUDevice::Instance()->GetNative()->CreateUnorderedAccessView(texture_.Get(), nullptr, &unordered_access_view);
		AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create unordered access view.");
		texture_view_.Attach(unordered_access_view);
	}
}

} // namespace aoe