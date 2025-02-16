#pragma once

#include "../ECS/World.h"
#include "../Core/Math.h"
#include "../Game/TransformUtils.h"

#include "Colors.h"
#include "DX11LineComponent.h"

namespace aoe {

class DebugUtils {
public:
	DebugUtils() = delete;

	static Entity CreateAxis(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Transform transfrom = {});

	static Entity CreateLine(
		World& world,
		std::vector<Vector3f> points,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

	static Entity CreateCircle(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

	static Entity CreateSquare(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

	static Entity CreateSphere(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

	static Entity CreateCube(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

	static Entity CreateGrid(
		World& world,
		Vector3i resolution,
		Transform transform = {},
		Vector3f color = Colors::kGreen);

private:
	static constexpr float kUnit = 1.0f;

	static Entity CreateTransformEntity(World& world, Transform transform);

	static std::vector<Vector3f> CreateCirclePoints(
		float radius, 
		Vector3f offset = Math::kZeros3f, 
		Vector3f angles = Math::kZeros3f);

	static std::vector<Vector3f> CreateSquarePoints(
		float side, 
		Vector3f offset = Math::kZeros3f,
		Vector3f angles = Math::kZeros3f);

	static Vector3f Transformate(const Vector3f& point, const Matrix4f transformation);
};

} // namespace aoe