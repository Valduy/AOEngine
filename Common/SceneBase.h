#pragma once

#include "../Application/Application.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformComponent.h"
#include "../Game/ServiceProvider.h"
#include "../Game/SystemsPool.h"
#include "../ECS/World.h"
#include "../Renderer/DX11RenderContext.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

namespace aoe {

class SceneBase : public IScene {
public:
	SceneBase(Application& application);

	void Initialize() override;
	void Terminate() override;

	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

protected:
	virtual void SetupServices(ServiceProvider& service_provider);
	virtual void InitializeTickSystems(SystemsPool& tick_systems_pool);
	virtual void InitializeFrameSystems(SystemsPool& frame_systems_pool);
	virtual void SetupScene(ServiceProvider& service_provider);

private:
	Application& application_;

	World world_;
	Relationeer<TransformComponent> relationeer_;

	DX11RenderContext render_context_;
	DX11ModelManager model_manager_;
	DX11TextureManager texture_manager_;

	ServiceProvider service_provider_;
	SystemsPool tick_systems_pool_;
	SystemsPool frame_systems_pool_;
};

} // namespace aoe