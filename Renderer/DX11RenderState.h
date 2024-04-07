#pragma once

#include <stack>

#include "../Core/Debug.h"
#include "../Graphics/DX11GPUContext.h"

namespace aoe {

class DX11RenderState {
public:
	bool IsEmpty() const {
		return render_targets_.empty();
	}

	void Push(DX11GPURenderTargets render_targets) {
		render_targets_.push(render_targets);
		SetupTop();
	}

	void Pop() {
		render_targets_.pop();
		
		if (!IsEmpty()) {
			SetupTop();
		}
	}

	const DX11GPURenderTargets& Peek() const {
		AOE_ASSERT_MSG(!IsEmpty(), "Render targets stack is empty.");
		return render_targets_.top();
	}

	void Clear() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		const DX11GPURenderTargets& actual_render_targets = Peek();
		
		for (size_t i = 0; i < DX11GPURenderTargets::kRenderTargetsCount; ++i) {
			const DX11GPUTextureView* render_target_view = actual_render_targets.render_target_views[i];

			if (render_target_view == nullptr) {
				continue;
			}

			context.ClearRenderTarget(*render_target_view, kEmptyColor);
		}

		if (actual_render_targets.depth_stencil_view != nullptr) {
			const DX11GPUTextureView* depth_stencil_view = actual_render_targets.depth_stencil_view;
			context.ClearDepth(*depth_stencil_view, 1.0f);
		}
	}

private:
	const float kEmptyColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	std::stack<DX11GPURenderTargets> render_targets_;

	void SetupTop() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRenderTargets(Peek());
	}
};

} // namespace aoe