#include <unordered_set>
#include <queue>
#include <algorithm>

#include "../Renderer/DX11RenderComponent.h"
#include "../Renderer/DX11AmbientLightComponent.h"
#include "../Renderer/DX11DirectionalLightComponent.h"
#include "../Renderer/DebugUtils.h"
#include "../Common/SceneBase.h"
#include "../Common/FlyCameraTickSystem.h"
#include "../Common/FlyCameraFrameSystem.h"

#include "Algorithms.h"

using NodeId = size_t;
using NodesCollection = std::unordered_set<NodeId>;

template<typename TData>
class Graph {
private:
	class Node {
		friend class Graph<TData>;
	public:
		template<typename... TArgs>
		Node(TArgs&&... args)
			: data(std::forward<TArgs>(args)...)
			, arcs()
		{}

		TData& GetData() {
			return data;
		}

		const TData& GetData() const {
			return data;
		}

		const NodesCollection& GetArcs() const {
			return arcs;
		}

	private:
		TData data;
		NodesCollection arcs;
	};

public:
	using Iterator = std::unordered_map<NodeId, Node>::iterator;
	using ConstIterator = std::unordered_map<NodeId, Node>::const_iterator;

	void AddNode(NodeId id, const TData& data) {
		EmplaceNode(id, data);
	}

	void AddNode(NodeId id, TData&& data) {
		EmplaceNode(id, std::move(data));
	}

	template<typename... TArgs>
	void EmplaceNode(NodeId id, TArgs&&... args) {
		auto result = nodes_.try_emplace(id, std::forward<TArgs>(args)...);

		if (result.second) {
			nodes_ids_.insert(id);
		}
		else {
			AOE_ASSERT_MSG(false, "Node `{}` already exists.", id);
		}
	}

	void RemoveNode(NodeId id) {
		auto it = nodes_.find(id);

		if (it != nodes_.end()) {
			nodes_.erase(it);
			nodes_ids_.erase(id);
		}

		for (auto& [_, data] : nodes_) {
			data.arcs.erase(id);
		}
	}

	const NodesCollection& GetNodesIds() const {
		return nodes_ids_;
	}

	const NodesCollection& GetArcs(NodeId id) const {
		const Node& node = nodes_.at(id);
		return node.arcs;
	}

	TData& GetData(NodeId id) {
		Node& node = nodes_.at(id);
		return node.data;
	}

	const TData& GetData(NodeId id) const {
		const Node& node = nodes_.at(id);
		return node.data;
	}

	bool HasArc(NodeId from, NodeId to) const {
		const NodesCollection& arcs = GetArcs(from);
		return arcs.contains(to);
	}

	void CreateArc(NodeId from, NodeId to) {
		if (!HasArc(from, to)) {
			Node& node = nodes_.at(from);
			node.arcs.insert(to);
		}
	}

	void BreakArc(NodeId from, NodeId to) {
		Node& node = nodes_.at(from);
		node.arcs.erase(to);
	}

	bool HasEdge(NodeId lhs, NodeId rhs) {
		return HasArc(lhs, rhs)
			&& HasArc(rhs, lhs);
	}

	void CreateEdge(NodeId lhs, NodeId rhs) {
		CreateArc(lhs, rhs);
		CreateArc(rhs, lhs);
	}

	void BreakEdge(NodeId lhs, NodeId rhs) {
		BreakArc(lhs, rhs);
		BreakArc(rhs, lhs);
	}

	Iterator begin() {
		return nodes_.begin();
	}

	Iterator end() {
		return nodes_.end();
	}

	ConstIterator cbegin() const {
		return nodes_.cbegin();
	}

	ConstIterator cend() const {
		return nodes_.cend();
	}

	Node& operator[](NodeId id) {
		return nodes_.at(id);
	}

	const Node& operator[](NodeId id) const {
		return nodes_.at(id);
	}

private:
	NodesCollection nodes_ids_;
	std::unordered_map<NodeId, Node> nodes_;
};

class PerlinNoise2D {
public:
	PerlinNoise2D(aoe::Random& random, size_t size) 
		: size_(size)
		, permutation_()
	{
		AOE_ASSERT_MSG(size * 2 <= std::numeric_limits<size_t>::max(), "To big size.");

		permutation_.reserve(size * 2);

		for (size_t i = 0; i < size; i++) {
			permutation_.push_back(i);
		}
		
		std::shuffle(permutation_.begin(), permutation_.end(), random.GetEngine());

		for (size_t i = 0; i < size; i++) {
			permutation_.push_back(i);
		}
	}

	float GetValue(float x, float y) const {
		AOE_ASSERT_MSG(x >= 0, "Argument x can't be negative.");
		AOE_ASSERT_MSG(y >= 0, "Argument y can't be negative.");

		const int floor_x = static_cast<int>(aoe::Math::Floor(x));
		const int floor_y = static_cast<int>(aoe::Math::Floor(y));

		const int wrap_x = floor_x % size_;
		const int wrap_y = floor_y % size_;

		const float xf = x - floor_x;
		const float yf = y - floor_y;

		// Top - t; Bottom - b; Left - l; Right - r.
		const aoe::Vector2f t_r_vector(xf - 1.0f, yf - 1.0f);
		const aoe::Vector2f t_l_vector(xf, yf - 1.0f);
		const aoe::Vector2f b_r_vector(xf - 1.0f, yf);
		const aoe::Vector2f b_l_vector(xf, yf);

		const size_t t_r_value = permutation_[permutation_[wrap_x + 1] + wrap_y + 1];
		const size_t t_l_value = permutation_[permutation_[wrap_x] + wrap_y + 1];
		const size_t b_r_value = permutation_[permutation_[wrap_x + 1] + wrap_y];
		const size_t b_l_value = permutation_[permutation_[wrap_x] + wrap_y];
		
		const float t_r_dot = aoe::Vector2f::DotProduct(t_r_vector, GetConstantVector(t_r_value));
		const float t_l_dot = aoe::Vector2f::DotProduct(t_l_vector, GetConstantVector(t_l_value));
		const float b_r_dot = aoe::Vector2f::DotProduct(b_r_vector, GetConstantVector(b_r_value));
		const float b_l_dot = aoe::Vector2f::DotProduct(b_l_vector, GetConstantVector(b_l_value));

		const float u = Fade(xf);
		const float v = Fade(yf);

		return aoe::Math::Lerp(
			aoe::Math::Lerp(b_l_dot, t_l_dot, v),
			aoe::Math::Lerp(b_r_dot, t_r_dot, v), 
			u);
	}

	float operator()(float x, float y) const {
		return GetValue(x, y);
	}

private:
	int size_;
	std::vector<size_t> permutation_;

	static aoe::Vector2f GetConstantVector(size_t corner) {
		switch (corner % 4) {
		case 0:
			return { 1.0f, 1.0f };
		case 1:
			return { -1.0f, 1.0f };
		case 2:
			return { -1.0f, -1.0f };
		case 3:
			return { 1.0f, -1.0f };
		default: 
			AOE_ASSERT_MSG(false, "Unreachable code.");
			return aoe::Math::kZeros2f;
		}
	}

	static float Fade(float t) {
		return ((6 * t - 15) * t + 10) * t * t * t;
	}
};

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

		//DebugUtils::CreateGrid(world, { 20, 0, 20 }, {}, Colors::kWhite);
		//DebugUtils::CreateAxis(world, relationeer);

		CreateAmbientLight(world);
		CreateDirectionalLight(world, Math::kDown + Math::kForward * 0.5f);

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

		// Test 4
		aoe::Random random(2);
		std::vector<Vector2f> random_points = Algorithms::PoissonDisk2D(
			random, { -half_edge, -half_edge }, { half_edge, half_edge }, 0.4f, 5);

		aoe::Vector2f centroid = Algorithms::FindCentroid(random_points.begin(), random_points.end());

		auto it = std::remove_if(random_points.begin(), random_points.end(),
			[&centroid](const aoe::Vector2f p) {
				return aoe::Vector2f::Distance(centroid, p) > half_edge;
			});
		random_points.erase(it, random_points.end());

		for (const Vector2f& point : random_points) {
			Entity sphere = CreateSphere(world, model_manager, texture_manager);
			auto transform_component = world.GetComponent<TransformComponent>(sphere);
			transform_component->SetPosition({ point.x, 0.0f, point.y });
			transform_component->SetScale({ 0.2f, 0.2f, 0.2f });
		}

		std::vector<Triangle2D> triangulation = Algorithms::BowyerWatson(random_points);
		Graph<Triangle2D> graph = CreateTriangulationGraph(triangulation);
		std::vector<NodeId> outline = FindOutline(graph);
		
		//for (const Triangle2D& triangle : triangulation) {
		//	CreateTriangle(world, triangle);
		//}

		//std::sort(outline.begin(), outline.end(), 
		//	[&triangulation, &centroid](const NodeId& lhs, const NodeId& rhs) {
		//		const Triangle2D& lhs_triangle = triangulation[lhs];
		//		const aoe::Vector2f lhs_centroid = Algorithms::FindCentroid(
		//			lhs_triangle.vertices.begin(), 
		//			lhs_triangle.vertices.end());

		//		const Triangle2D& rhs_triangle = triangulation[rhs];
		//		const aoe::Vector2f rhs_centroid = Algorithms::FindCentroid(
		//			rhs_triangle.vertices.begin(),
		//			rhs_triangle.vertices.end());

		//		const int lhs_sign = aoe::Math::Sign(lhs_centroid.y);
		//		const int rhs_sign = aoe::Math::Sign(rhs_centroid.y);

		//		if (lhs_sign != rhs_sign) {
		//			return lhs_sign > rhs_sign;
		//		}

		//		const aoe::Vector2f axis = aoe::Math::kAxisX2f;
		//		const aoe::Vector2f lhs_direction = (lhs_centroid - centroid).Normalized();
		//		const aoe::Vector2f rhs_direction = (rhs_centroid - centroid).Normalized();

		//		const float lhs_angle = aoe::Math::Acos(aoe::Vector2f::DotProduct(axis, lhs_direction));
		//		const float rhs_angle = aoe::Math::Acos(aoe::Vector2f::DotProduct(axis, rhs_direction));

		//		if (lhs_sign > 0) {
		//			return lhs_angle < rhs_angle;
		//		} else {
		//			return lhs_angle > rhs_angle;
		//		}
		//	});

		//const float color_step = 1.0f / outline.size();
		//float color_factor_b = 0;

		//for (size_t i = 0; i < outline.size(); ++i) {
		//	const NodeId id = outline[i];
		//	const Triangle2D& triangle = triangulation[id];
		//	const Vector2f triangle_centroid = Algorithms::FindCentroid(
		//		triangle.vertices.begin(), 
		//		triangle.vertices.end());

		//	aoe::DebugUtils::CreateLine(
		//		world,
		//		{ { centroid.x, 0.0f, centroid.y }, { triangle_centroid.x, 0.0f, triangle_centroid.y } },
		//		{},
		//		{ 1.0f, 0.0f, color_factor_b });

		//	color_factor_b += color_step;
		//}

		for (NodeId id : outline) {
			graph.RemoveNode(id);
		}

		std::vector<NodeId> roots = SelectRandomRoots(graph, random, 3);
		std::vector<std::vector<NodeId>> plates = DistributeTrianglesBetweenPlates(graph, roots);

		for (size_t i = 0; i < plates.size(); ++i) {
			std::vector<NodeId>& plate = plates[i];
			aoe::Vector3f color = aoe::Math::kZeros3f;
			color[i] = 1.0f;

			for (NodeId id : plate) {
				const Triangle2D& triangle = graph.GetData(id);
				CreateTriangle(world, triangle, color);
			}
		}

		const size_t size = 8;
		const size_t divs = 8;
		const size_t res = size * divs;

		float map[res][res];
		PerlinNoise2D noise(random, size);
		
		for (size_t x = 0; x < size; ++x) {
			for (size_t y = 0; y < size; ++y) {
				float offset = 1.0f / divs;
				
				for (size_t div_x = 0; div_x < divs; ++div_x) {
					for (size_t div_y = 0; div_y < divs; ++div_y) {
						const float i = static_cast<float>(x) + offset + offset * div_x;
						const float j = static_cast<float>(y) + offset + offset * div_y;
						
						float value = noise(i, j);
						map[divs * x + div_x][divs * y + div_y] = value;
					}
				}
			}
		}

		for (size_t x = 0; x < res - 1; ++x) {
			for (size_t y = 0; y < res - 1; ++y) {
				float value = map[x][y];
				Entity cube = CreateCube(world, model_manager, texture_manager);
				auto transform_component = world.GetComponent<TransformComponent>(cube);
				Vector3f position(static_cast<float>(half_edge + x), 2 * value, static_cast<float>(half_edge + y));
				transform_component->SetPosition(position);
			}
		}
	}

	void PerTickUpdate(float dt) override {
		AOE_LOG_INFO("{}", dt);
		aoe::SceneBase::PerTickUpdate(dt);
	}

	void PerFrameUpdate(float dt) override {
		aoe::SceneBase::PerFrameUpdate(dt);
	}

	static Graph<Triangle2D> CreateTriangulationGraph(const std::vector<Triangle2D>& triangulation) {
		Graph<Triangle2D> graph;

		for (NodeId id = 0; id < triangulation.size(); ++id) {
			const Triangle2D& triangle = triangulation[id];
			graph.AddNode(id, triangle);
		}

		for (NodeId i = 0; i < triangulation.size(); ++i) {
			const Triangle2D& node = triangulation[i];
			
			for (NodeId j = i + 1; j < triangulation.size(); ++j) {
				const Triangle2D& neighbour = triangulation[j];

				for (const Edge2D& edge : node.ToEdges()) {
					if (neighbour.HasEdge(edge)) {
						graph.CreateEdge(i, j);
						break;
					}
				}
			}
		}

		return graph;
	}

	static std::vector<NodeId> FindOutline(const Graph<Triangle2D>& graph) {
		std::vector<NodeId> outline;
		const NodesCollection& nodes_ids_ = graph.GetNodesIds();

		for (NodeId id : nodes_ids_) {
			const NodesCollection arcs = graph.GetArcs(id);

			if (arcs.size() < 3) {
				outline.push_back(id);
			}
		}

		return outline;
	}

	struct PlateDistributionState {
		std::unordered_set<NodeId> visited;
		std::queue<NodeId> frontier;
	};

	static std::vector<NodeId> SelectRandomRoots(
		const Graph<Triangle2D>& graph,
		aoe::Random& random,
		size_t count) 
	{
		std::vector<NodeId> roots(count);
		const NodesCollection& nodes = graph.GetNodesIds();
		auto begin = nodes.cbegin();

		for (size_t i = 0; i < count; ++i) {
			int number = random.Next(static_cast<int>(nodes.size() - 1));
			auto it = std::next(begin, number);
			roots[i] = *it;
		}

		return roots;
	}

	static std::vector<std::vector<NodeId>> DistributeTrianglesBetweenPlates(
		const Graph<Triangle2D>& graph, 
		const std::vector<NodeId>& roots) 
	{
		std::vector<std::vector<NodeId>> plates;
		std::unordered_set<NodeId> distribution;
		std::vector<PlateDistributionState> states(roots.size());

		for (size_t i = 0; i < roots.size(); ++i) {
			NodeId id = roots[i];
			PlateDistributionState& state = states[i];
			state.frontier.push(id);
		}

		bool is_finished = false;

		while (!is_finished) {
			is_finished = true;

			for (PlateDistributionState& state : states) {
				is_finished &= !ProcessPlateDistribution(graph, state, distribution);
			}
		}

		for (PlateDistributionState& state : states) {
			std::vector<NodeId>& plate = plates.emplace_back();
			plate.assign(state.visited.begin(), state.visited.end());
		}

		return plates;
	}

	static bool ProcessPlateDistribution(
		const Graph<Triangle2D>& graph,
		PlateDistributionState& state,
		std::unordered_set<NodeId>& distribution)
	{
		while (!state.frontier.empty()) {
			NodeId node = state.frontier.front();
			state.frontier.pop();

			if (distribution.contains(node)) {
				continue;
			}

			distribution.insert(node);
			state.visited.insert(node);
			
			for (NodeId neighbour : graph.GetArcs(node)) {
				if (!state.visited.contains(neighbour)) {
					state.frontier.push(neighbour);
				}
			}

			return true;
		}

		return false;
	}

	static aoe::Entity CreateAmbientLight(aoe::World& world) {
		using namespace aoe;

		Entity ambient_light = world.CreateEntity();
		world.AddComponent<DX11AmbientLightComponent>(ambient_light, Vector3f(0.1f, 0.1f, 0.1f));

		return ambient_light;
	}

	static aoe::Entity CreateDirectionalLight(aoe::World& world, aoe::Vector3f direction) {
		using namespace aoe;

		Entity directional_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(directional_light);
		world.AddComponent<DX11DirectionalLightComponent>(directional_light, Vector3f(1.0f, 1.0f, 1.0f));

		auto transform_component = world.GetComponent<TransformComponent>(directional_light);

		Transform transform = transform_component->GetTransform();
		Quaternion rotation = Quaternion::RotateFromTo(transform.GetForward(), direction.Normalized());
		transform_component->SetRotation(rotation);

		return directional_light;
	}

	static aoe::Entity CreateSphere(
		aoe::World& world, 
		aoe::DX11ModelManager& model_manager,
		aoe::DX11TextureManager& texture_manager) 
	{
		using namespace aoe;

		ModelId model_id = model_manager.Load(L"Content/Sphere.fbx", ModelLoaderOptions::kFlipUVs);
		TextureId texture_id = texture_manager.kDefault;

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		Entity sphere = world.CreateEntity();
		world.AddComponent<TransformComponent>(sphere);
		world.AddComponent<DX11RenderComponent>(sphere, model_id, texture_id, material);

		return sphere;
	}

	static aoe::Entity CreateCube(
		aoe::World& world,
		aoe::DX11ModelManager& model_manager,
		aoe::DX11TextureManager& texture_manager)
	{
		using namespace aoe;

		ModelId model_id = model_manager.Load(L"Content/Cube.fbx", ModelLoaderOptions::kFlipUVs);
		TextureId texture_id = texture_manager.kDefault;

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		Entity cube = world.CreateEntity();
		world.AddComponent<TransformComponent>(cube);
		world.AddComponent<DX11RenderComponent>(cube, model_id, texture_id, material);

		return cube;
	}

	static aoe::Entity CreateTriangle(
		aoe::World& world, 
		const Triangle2D& triangle, 
		aoe::Vector3f color = aoe::Colors::kGreen) 
	{
		using namespace aoe;

		std::vector<Vector3f> line_points(4);
		std::transform(triangle.vertices.begin(), triangle.vertices.end(), line_points.begin(),
			[](const Vector2f& v) {
				return Vector3f(v.x, 0.0f, v.y);
			});
		line_points[3] = line_points.front();

		return DebugUtils::CreateLine(world, line_points, {}, color);
	}
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	LandscaperScene scene(application);
	application.Start(scene);

	return 0;
}