#pragma once

namespace aoe {

template<typename TResource>
class GPUResourceWrapper {
public:
	TResource* GetNative() const;

	GPUResourceWrapper(TResource* resource);

private:
	TResource* resource_;
};

template<typename TResource>
TResource* GPUResourceWrapper<TResource>::GetNative() const {
	return resource_;
}

template<typename TResource>
GPUResourceWrapper<TResource>::GPUResourceWrapper(TResource* resource)
	: resource_(resource)
{}

} // namespace aoe