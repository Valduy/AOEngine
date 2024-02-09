#pragma once

#include <wrl/client.h>

#include "DX11Headers.h"

namespace aoe {

class DX11GPUTexture2D;

class DX11GPUTextureView {
public:
	bool IsShaderResourceView() const {
		return GetShaderResourceView() != nullptr;
	}

	bool IsDepthStencilView() const {
		return GetDepthStencilView() != nullptr;
	}

	bool IsRenderTargetView() const {
		return GetRenderTargetView() != nullptr;
	}

	bool IsUnorderedAccessView() const {
		return GetUnorderedAccessView() != nullptr;
	}

	ID3D11ShaderResourceView* GetShaderResourceView() const {
		return shader_resource_view_.Get();
	}

	ID3D11DepthStencilView* GetDepthStencilView() const {
		return depth_stencil_view_.Get();
	}

	ID3D11RenderTargetView* GetRenderTargetView() const {
		return render_target_view_.Get();
	}

	ID3D11UnorderedAccessView* GetUnorderedAccessView() const {
		return unordered_access_view_.Get();
	}

	DX11GPUTextureView(DX11GPUTexture2D* texture)
		: texture_(texture)
		, shader_resource_view_(nullptr)
		, depth_stencil_view_(nullptr)
		, render_target_view_(nullptr)
		, unordered_access_view_(nullptr)
	{}

	DX11GPUTexture2D* GetTexture() const {
		return texture_;
	}

	void Attach(DX11GPUTexture2D* texture) {
		texture_ = texture;
	}

	void Attach(ID3D11ShaderResourceView* shader_resource_view) {
		shader_resource_view_.Attach(shader_resource_view);
	}

	void Attach(ID3D11DepthStencilView* depth_stencil_view) {
		depth_stencil_view_.Attach(depth_stencil_view);
	}

	void Attach(ID3D11RenderTargetView* render_target_view) {
		render_target_view_.Attach(render_target_view);
	}

	void Attach(ID3D11UnorderedAccessView* unordered_access_view) {
		unordered_access_view_.Attach(unordered_access_view);
	}

private:
	DX11GPUTexture2D* texture_;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unordered_access_view_;
};

} // namespace aoe