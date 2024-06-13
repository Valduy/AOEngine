#pragma once

#include "../ECS/World.h"
#include "../Game/ServiceProvider.h"
#include "../Game/TransformUtils.h"
#include "../Game/Relationeer.h"
#include "../Graphics/DX11GPUContext.h"

#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"

namespace aoe {

class DX11RenderPassBase {
public:
	DX11RenderPassBase()
		: world_(nullptr)
		, render_context_(nullptr)
		, relationeer_(nullptr)
	{}

	World* GetWorld() {
		return world_;
	}

	DX11RenderContext* GetRenderContext() {
		return render_context_;
	}

	Relationeer<TransformComponent>* GetRelationeer() {
		return relationeer_;
	}

	virtual void Initialize(const aoe::ServiceProvider& service_provider) {
		world_ = service_provider.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		render_context_ = service_provider.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");
	
		relationeer_ = service_provider.GetService<Relationeer<TransformComponent>>();
		AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");
	}

	virtual void Terminate() {}

	virtual void Update() {}
	virtual void Render() {}

private:
	World* world_;
	DX11RenderContext* render_context_;
	Relationeer<TransformComponent>* relationeer_;
};

} // namespace aoe