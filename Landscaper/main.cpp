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
	size_t size_;
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

namespace std {

	template<>
	struct hash<aoe::Vector2i> {
		size_t operator()(const aoe::Vector2i& value) const noexcept {
			size_t hash0 = hash<int>()(value.x);
			size_t hash1 = hash<int>()(value.y);
			return hash0 ^ hash1;
		}
	};

} // namespace std

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
		world.AddComponent<DX11CameraComponent>(camera);
		world.AddComponent<FlyCameraComponent>(camera);

		auto camera_component = world.GetComponent<DX11CameraComponent>(camera);
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
				return aoe::Vector2f::Distance(centroid, p) >= half_edge;
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

		const size_t divisions = 10;
		const size_t subdivisions = 10;
		const size_t side = subdivisions * divisions;
		const float max_height = 8;

		std::vector<Grid2D<float>> plates_noises = GeneratePlatesNoises(
			plates, graph, { -half_edge, -half_edge }, { half_edge, half_edge }, side);

		for (Grid2D<float>& plate_noise : plates_noises) {
			std::unordered_set<Vector2i> visited;
			std::queue<Vector2i> frontier;

			for (int x = 1; x < plate_noise.GetWidth() - 1; ++x) {
				for (int y = 1; y < plate_noise.GetHeight() - 1; ++y) {
					int factor = 
						static_cast<int>(plate_noise(x - 1, y) > 0) +
						static_cast<int>(plate_noise(x + 1, y) > 0) +
						static_cast<int>(plate_noise(x, y - 1) > 0) +
						static_cast<int>(plate_noise(x, y + 1) > 0);

					if (factor == 0) {
						continue;
					} else if (factor == 4) {
						visited.insert({ x, y });
					} else {
						frontier.push({ x, y });
					}
				}
			}

			while (!frontier.empty()) {
				const Vector2i position = frontier.front();
				frontier.pop();

				visited.insert(position);
				float propagate = plate_noise[position] * plate_noise[position] * 0.95f;

				if (propagate <= 0.01f) {
					continue;
				}

				if (position.x > 0 && plate_noise(position.x - 1, position.y) < propagate) {
					plate_noise[{ position.x - 1, position.y }] = propagate;
					frontier.push({ position.x - 1, position.y });
				} else if (position.x < plate_noise.GetWidth() - 1 && plate_noise(position.x + 1, position.y) < propagate) {
					plate_noise[{ position.x + 1, position.y }] = propagate;
					frontier.push({ position.x + 1, position.y });
				} else if (position.y > 0 && plate_noise(position.x, position.y - 1) < propagate) {
					plate_noise[{ position.x, position.y - 1 }] = propagate;
					frontier.push({ position.x, position.y - 1 });
				} else if (position.y < plate_noise.GetHeight() - 1 && plate_noise(position.x, position.y + 1) < propagate) {
					plate_noise[{ position.x, position.y + 1 }] = propagate;
					frontier.push({ position.x, position.y + 1 });
				}
			}
		}

		Grid2D<float> noise(side, side);

		for (size_t x = 0; x < noise.GetWidth(); ++x) {
			for (size_t y = 0; y < noise.GetHeight(); ++y) {
				noise(x, y) = 0.0f;
			}
		}

		for (size_t i = 0; i < 3; ++i) {
			Grid2D<float> random_noise = GenerateNoise(random, divisions, subdivisions);

			for (size_t x = 0; x < random_noise.GetWidth(); ++x) {
				for (size_t y = 0; y < random_noise.GetHeight(); ++y) {
					random_noise(x, y) /= 2.0f;
				}
			}

			plates_noises.emplace_back(std::move(random_noise));
		}

		for (size_t i = 0; i < plates_noises.size(); ++i) {
			// if (i != 2) continue;

			Grid2D<float>& plate_noise = plates_noises[i];

			for (size_t x = 0; x < plate_noise.GetWidth(); ++x) {
				for (size_t y = 0; y < plate_noise.GetHeight(); ++y) {
					if (plate_noise(x, y) > 0) {
						noise(x, y) += plate_noise(x, y);
					}
				}
			}
		}

		for (size_t x = 0; x < noise.GetWidth(); ++x) {
			for (size_t y = 0; y < noise.GetHeight(); ++y) {
				if (noise(x, y) > 0) {
					noise(x, y) /= plates_noises.size();
				}
			}
		}

		//Grid2D<float> noise = GenerateNoise(random, divisions, subdivisions);
		Grid2D<Vector3f> positions = NoiseToPositions(noise, max_height);

		ModelId model_id = GenerateTerrainMesh(model_manager, positions);
		TextureId texture_id = texture_manager.LoadRGBA(L"Content/Dirt.png");

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.1f, 0.1f, 0.1f };
		material.shininess = 2.0f;

		Entity sphere = world.CreateEntity();
		world.AddComponent<TransformComponent>(sphere);
		world.AddComponent<DX11RenderComponent>(sphere, model_id, texture_id, material);
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
		const NodesCollection& nodes_ids = graph.GetNodesIds();

		for (NodeId id : nodes_ids) {
			const NodesCollection& arcs = graph.GetArcs(id);

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

	static std::vector<Grid2D<float>> GeneratePlatesNoises(
		const std::vector<std::vector<NodeId>>& plates,
		const Graph<Triangle2D>& graph,
		const aoe::Vector2f& min,
		const aoe::Vector2f& max,
		size_t resolution) 
	{
		std::vector<Grid2D<float>> result;
		result.reserve(plates.size());

		for (const std::vector<NodeId>& plate : plates) {
			Grid2D<float> noise = GeneratePlateNoise(plate, graph, min, max, resolution);
			result.emplace_back(std::move(noise));			
		}

		CleanupPlatesNoisesIntersections(result);
		return result;
	}

	static Grid2D<float> GeneratePlateNoise(
		const std::vector<NodeId>& plate,
		const Graph<Triangle2D>& graph,
		const aoe::Vector2f& min,
		const aoe::Vector2f& max,
		size_t resolution) 
	{
		using namespace aoe;

		Grid2D<float> result(resolution, resolution);

		for (size_t x = 0; x < result.GetWidth(); ++x) {
			const float point_x = Math::Map(x, 0, result.GetWidth(), min.x, max.x);

			for (size_t y = 0; y < result.GetHeight(); ++y) {
				const float point_y = Math::Map(y, 0, result.GetHeight(), min.y, max.y);
				const Vector2f point(point_x, point_y);
				result(x, y) = 0.0f;

				for (NodeId id : plate) {
					const Triangle2D& triangle = graph.GetData(id);
					int factor = 0;

					for (const Edge2D& edge : triangle.ToEdges()) {
						const Vector2f to_vertex = edge.b - edge.a;
						const Vector2f to_point = point - edge.a;

						factor += Math::Sign(Math::CrossProduct(to_vertex, to_point));
					}

					// This mean that point is inside of the triangle.
					if (Math::Abs(factor) == 3) {
						result(x, y) = 1.0f;
						break;
					}
				}
			}
		}

		return result;
	}

	static void CleanupPlatesNoisesIntersections(std::vector<Grid2D<float>> plates_noises) {
		for (size_t i = 0; i < plates_noises.size(); ++i) {
			const Grid2D<float>& stencil = plates_noises[i];

			for (size_t j = i + 1; j < plates_noises.size(); ++j) {
				Grid2D<float>& plate_noise = plates_noises[j];

				for (size_t x = 0; x < plate_noise.GetWidth(); ++x) {
					for (size_t y = 0; y < plate_noise.GetHeight(); ++y) {
						if (stencil(x, y) > 0 && plate_noise(x, y) > 0) {
							plate_noise(x, y) = 0.0f;
						}
					}
				}
			}
		}
	}

	static Grid2D<float> GenerateNoise(aoe::Random& random, const size_t divisions, const size_t subdivisions) {
		const size_t resolution = divisions * subdivisions;
		Grid2D<float> noise(resolution, resolution);
		PerlinNoise2D generator(random, divisions);

		for (size_t division_x = 0; division_x < divisions; ++division_x) {
			for (size_t division_y = 0; division_y < divisions; ++division_y) {
				float step = 1.0f / subdivisions;

				for (size_t subdivision_x = 0; subdivision_x < subdivisions; ++subdivision_x) {
					for (size_t subdivision_y = 0; subdivision_y < subdivisions; ++subdivision_y) {
						const float xf = step * subdivision_x;
						const float yf = step * subdivision_y;
						
						const size_t x = subdivisions * division_x + subdivision_x;
						const size_t y = subdivisions * division_y + subdivision_y;

						noise(x, y) = (generator(xf + division_x, yf + division_y) + 1.0f) / 2.0f;
					}
				}
			}
		}

		return noise;
	}

	static Grid2D<aoe::Vector3f> NoiseToPositions(const Grid2D<float>& noise, const float max_height) {
		Grid2D<aoe::Vector3f> positions(noise.GetWidth(), noise.GetHeight());

		for (size_t x = 0; x < noise.GetWidth(); ++x) {
			for (size_t y = 0; y < noise.GetHeight(); ++y) {
				const float position_x = static_cast<float>(x);
				const float position_y = noise(x, y) * max_height;
				const float position_z = static_cast<float>(y);

				positions(x, y) = { position_x, position_y, position_z };
			}
		}

		return positions;
	}

	static aoe::ModelId GenerateTerrainMesh(
		aoe::DX11ModelManager& model_manager,
		const Grid2D<aoe::Vector3f>& positions) 
	{
		using namespace aoe;

		std::vector<Vertex> vertices;
		std::vector<Index> indices;
		std::vector<Vector3f> normals;

		for (size_t x = 0; x < positions.GetWidth(); ++x) {
			for (size_t y = 0; y < positions.GetHeight(); ++y) {
				normals.clear();
				aoe::Vector3f a = positions(x, y);

				if (y > 1 && x > 1) {
					Vector3f b = positions(x - 1, y);
					Vector3f c = positions(x, y - 1);
					normals.emplace_back(Vector3f::CrossProduct(a - b, c - b));
				}
				if (y > 1 && x < positions.GetWidth() - 1) {
					Vector3f b = positions(x, y - 1);
					Vector3f c = positions(x + 1, y);
					normals.emplace_back(Vector3f::CrossProduct(a - b, c - b));
				}
				if (y < positions.GetHeight() - 1 && x < positions.GetWidth() - 1) {
					Vector3f b = positions(x + 1, y);
					Vector3f c = positions(x, y + 1);
					normals.emplace_back(Vector3f::CrossProduct(a - b, c - b));
				}
				if (y < positions.GetHeight() - 1 && x > 1) {
					Vector3f b = positions(x, y + 1);
					Vector3f c = positions(x - 1, y);
					normals.emplace_back(Vector3f::CrossProduct(a - b, c - b));
				}

				Vector3f averaged_normal = normals[0];

				for (size_t k = 1; k < normals.size(); ++k) {
					const Vector3f& normal = normals[k];
					averaged_normal = Vector3f::Lerp(averaged_normal, normal, 0.5f);
				}

				const float u = static_cast<float>(x % 2);
				const float v = static_cast<float>(y % 2);

				Vector3f position = positions(x, y);
				Vector3f normal = averaged_normal.Normalized();
				Vector2f uv = { u, v };

				Vertex vertex(position, normal, uv);
				vertices.push_back(vertex);

				if (x < positions.GetWidth() - 1) {
					if (y >= 1) {
						// c---d
						// | \ |
						// a---b

						size_t c = positions.GetHeight() * x + y;
						size_t a = c - 1;
						size_t d = c + positions.GetHeight();
						size_t b = a + positions.GetHeight();

						indices.push_back(c);
						indices.push_back(b);
						indices.push_back(a);

						indices.push_back(b);
						indices.push_back(c);
						indices.push_back(d);
					}
				}
			}
		}

		Mesh mesh(std::move(vertices), std::move(indices));
		Model model({ std::move(mesh) });
		ModelId model_id = model_manager.Upload(model);

		return model_id;
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
		TextureId texture_id = texture_manager.LoadRGBA(L"Content/Dirt.png");

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.4f, 0.4f, 0.4f };
		material.shininess = 12.0f;

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