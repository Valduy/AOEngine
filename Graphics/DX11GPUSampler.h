#pragma once

#include "IGPUSampler.h"
#include "DX11GPUDevice.h"

namespace aoe {

class DX11GPUSampler : public IGPUSampler {
public:
	void* GetNative() const override;
	const GPUSamplerDescription& GetDescription() const override;

	DX11GPUSampler(const DX11GPUDevice& device, const GPUSamplerDescription& description);

private:
	const DX11GPUDevice& device_;
	GPUSamplerDescription description_;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;

	static D3D11_FILTER ToDXFilter(const GPUSamplerFilter filter, const GPUSamplerComparsionFunction comparsion_function);
	static D3D11_COMPARISON_FUNC ToDXComparsionFunction(const GPUSamplerComparsionFunction comparsion_function);
	static D3D11_TEXTURE_ADDRESS_MODE ToDXAddressMode(const GPUSamplerAddressMode address_mode);
	static void FillBorderColor(const GPUSamplerBorderColor border_color, float color[4]);
};

} // namespace aoe