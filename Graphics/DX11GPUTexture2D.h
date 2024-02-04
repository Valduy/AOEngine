#pragma once

#include "IGPUTexture2D.h"
#include "DX11GPUDevice.h"
#include "DX11GPUTextureView.h"

namespace aoe {

class DX11GPUTexture2D : public IGPUTexture2D {
public:
	template<typename TElement>
	static DX11GPUTexture2D Create(const DX11GPUDevice& device, const GPUTexture2DDescription& description, const TElement* data);
	static DX11GPUTexture2D Create(const DX11GPUDevice& device, const GPUTexture2DDescription& description);

	void* GetNative() const override;
	const GPUTexture2DDescription& GetDescription() const override;
	virtual const IGPUTextureView* GetTextureView() const override;

	uint32_t GetWidth() const override;
	uint32_t GetHeight() const override;

	GPUPixelFormat GetPixelFormat() const override;

	bool IsShaderResource() const override;
	bool IsDepthStencil() const override;
	bool IsRenderTarget() const override;
	bool IsUnorderedAccess() const override;

	DX11GPUTexture2D(const DX11GPUDevice& device, const GPUTexture2DDescription& description, const void* data, size_t stride);
	DX11GPUTexture2D(const DX11GPUDevice& device, ID3D11Texture2D* texture);

private:
	const DX11GPUDevice& device_;
	GPUTexture2DDescription description_;
	DX11GPUTextureView texture_view_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
	
	static uint32_t ToDXBindFlag(GPUTextureFlags value);
	static GPUTextureFlags FromDXBindFlag(uint32_t value);

	void CreateTextureViews();
};

template<typename TElement>
static DX11GPUTexture2D DX11GPUTexture2D::Create(const DX11GPUDevice& device, const GPUTexture2DDescription& description, const TElement* data) {
	return { device, description, data, sizeof(TElement) };
}

} // namespace aoe