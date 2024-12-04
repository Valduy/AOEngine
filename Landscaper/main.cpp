#include "../Renderer/RenderComponent.h"
#include "../Renderer/DirectionalLightComponent.h"
#include "../Renderer/DebugUtils.h"
#include "../Common/SceneBase.h"
#include "../Common/FlyCameraTickSystem.h"
#include "../Common/FlyCameraFrameSystem.h"

#include "Algorithms.h"

class LandscaperScene : public aoe::SceneBase {
public:
	LandscaperScene(aoe::Application& application)
		: aoe::SceneBase(application)
	{}

protected:
	void InitializeTickSystems(aoe::SystemsPool& tick_systems_pool) override {
		tick_systems_pool.PushSystem<aoe::FlyCameraTickSystem>();

		aoe::SceneBase::InitializeTickSystems(tick_systems_pool);
	}

	void InitializeFrameSystems(aoe::SystemsPool& frame_systems_pool) override {
		frame_systems_pool.PushSystem<aoe::FlyCameraFrameSystem>();
		
		aoe::SceneBase::InitializeFrameSystems(frame_systems_pool);
	}

	void SetupScene(aoe::ServiceProvider& service_provider) override {
		using namespace aoe;

		Application& application = service_provider.GetService<Application>();
		World& world = service_provider.GetService<World>();
		Relationeer<TransformComponent>& relationeer = service_provider.GetService<Relationeer<TransformComponent>>();
		DX11ModelManager& model_manager = service_provider.GetService<DX11ModelManager>();
		DX11TextureManager& texture_manager = service_provider.GetService<DX11TextureManager>();

		// DebugUtils::CreateGrid(world, { 20, 0, 20 }, {}, Colors::kWhite);
		// DebugUtils::CreateAxis(world, relationeer);

		CreateDirectionalLight(world, Math::kDown);

		Entity camera = world.CreateEntity();
		world.AddComponent<TransformComponent>(camera);
		world.AddComponent<CameraComponent>(camera);
		world.AddComponent<FlyCameraComponent>(camera);

		auto camera_component = world.GetComponent<CameraComponent>(camera);
		camera_component->projection = Projection::kPerspective;
		camera_component->width = application.GetWindow().GetWidth();
		camera_component->height = application.GetWindow().GetHeight();
		camera_component->near_plain = 0.1f;
		camera_component->far_plain = 100.0f;
		camera_component->field_of_view = Math::kPi / 2;

		auto fly_camera_component = world.GetComponent<FlyCameraComponent>(camera);
		fly_camera_component->speed = 6.0f;
		fly_camera_component->sensitivity = 0.002f;

		const int half_edge = 4;

		aoe::Random random(2);
		std::vector<Vector2f> random_points = Algorithms::PoissonDisk2D(
			random, { -half_edge, -half_edge }, { half_edge, half_edge }, 0.4f, 5);

		aoe::Vector2f centroid = Algorithms::FindCentroid(random_points);

		auto it = std::remove_if(random_points.begin(), random_points.end(),
			[&centroid](const aoe::Vector2f p) {
				return aoe::Vector2f::Distance(centroid, p) > half_edge;
			});
		random_points.erase(it, random_points.end());

		for (const Vector2f& point : random_points) {
			Entity sphere = CreateSphere(world, model_manager, texture_manager);
			auto transform_component = world.GetComponent<TransformComponent>(sphere);
			transform_component->transform.position = { point.x, 0.0f, point.y };
			transform_component->transform.scale = { 0.2f, 0.2f, 0.2f };
		}

		std::vector<Triangle2D> triangulation = Algorithms::BowyerWatson(random_points);

		for (const Triangle2D& triangle : triangulation) {
			CreateTriangle(world, triangle);
		}
	}

	static aoe::Entity CreateDirectionalLight(aoe::World& world, aoe::Vector3f direction) {
		using namespace aoe;

		Entity directional_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(directional_light);
		world.AddComponent<DirectionalLightComponent>(directional_light, Vector3f(1.0f, 1.0f, 1.0f));

		auto transform_component = world.GetComponent<TransformComponent>(directional_light);

		Transform& transform = transform_component->transform;
		transform.rotation = Quaternion::RotateFromTo(transform.GetForward(), direction.Normalized());

		return directional_light;
	}

	static aoe::Entity CreateSphere(
		aoe::World& world, 
		aoe::DX11ModelManager& model_manager,
		aoe::DX11TextureManager& texture_manager) 
	{
		using namespace aoe;

		ModelId model_id = model_manager.Load(L"/Content/Sphere.fbx", ModelLoaderOptions::kFlipUVs);
		TextureId texture_id = texture_manager.kDefault;

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		Entity sphere = world.CreateEntity();
		world.AddComponent<TransformComponent>(sphere);
		world.AddComponent<RenderComponent>(sphere, model_id, texture_id, material);

		return sphere;
	}

	static aoe::Entity CreateTriangle(aoe::World& world, const Triangle2D& triangle) {
		using namespace aoe;

		std::vector<Vector3f> line_points(4);
		std::transform(triangle.vertices.begin(), triangle.vertices.end(), line_points.begin(),
			[](const Vector2f& v) {
				return Vector3f(v.x, 0.0f, v.y);
			});
		line_points[3] = line_points.front();

		return DebugUtils::CreateLine(world, line_points);
	}
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	LandscaperScene scene(application);
	application.Start(scene);

	return 0;
}