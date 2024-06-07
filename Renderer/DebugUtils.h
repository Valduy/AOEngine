#pragma once

#include "../ECS/World.h"
#include "../Core/Math.h"
#include "../Game/TransformUtils.h"

#include "Colors.h"
#include "LineComponent.h"

namespace aoe {

struct AxisDescription {
	Vector3f position = Math::kZeros3f;
	Quaternion rotation = Math::kQIdentity;
	float scale = 1.0f;
};

struct DebugDescription {
	Vector3f position = Math::kZeros3f;
	Quaternion rotation = Math::kQIdentity;
	Vector3f scale = Math::kOnes3f;
	Vector3f color = Colors::kGreen;
};

class DebugUtils {
public:
	DebugUtils() = delete;

	static Entity CreateAxis(World& world, Relationeer<TransformComponent>& relationeer, const AxisDescription& desc = {}) {
		Vector3f scale = { desc.scale, desc.scale, desc.scale };
		Entity axis = CreateTransformEntity(world, desc.position, desc.rotation, scale);
		DebugDescription line_desc{};

		line_desc.color = Colors::kRed;
		Entity axis_x = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }, line_desc);
		relationeer.SetParent(axis_x, axis);

		line_desc.color = Colors::kGreen;
		Entity axis_y = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }, line_desc);
		relationeer.SetParent(axis_y, axis);

		line_desc.color = Colors::kBlue;
		Entity axis_z = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }, line_desc);
		relationeer.SetParent(axis_z, axis);
		
		return axis;
	}

	static Entity CreateLine(World& world, std::vector<Vector3f> points, const DebugDescription& desc = {}) {
		Entity line = CreateTransformEntity(world, desc.position, desc.rotation, desc.scale);
		world.Add<LineComponent>(line, desc.color, std::move(points));

		return line;
	}

	static Entity CreateCircle(World& world, Relationeer<TransformComponent>& relationeer, const DebugDescription& desc = {}) {
		return Entity::Null();
	}

	static Entity CreateSphere(DebugDescription desc) {
		return Entity::Null();
	}

private:
	static Entity CreateTransformEntity(World& world, Vector3f position, Quaternion rotation, Vector3f scale) {
		Entity entity = world.Create();
		world.Add<TransformComponent>(entity, position, rotation, scale);

		return entity;
	}
};

} // namespace aoe