#pragma once

#include "../ECS/World.h"
#include "../Core/Math.h"
#include "../Game/TransformUtils.h"

#include "Colors.h"
#include "LineComponent.h"

namespace aoe {

class DebugUtils {
public:
	DebugUtils() = delete;

	static Entity CreateAxis(
		World& world, 
		Relationeer<TransformComponent>& relationeer, 
		Transform transfrom) 
	{
		Entity axis = CreateTransformEntity(world, transfrom);
		
		Entity axis_x = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} }, {}, Colors::kRed);
		relationeer.SetParent(axis_x, axis);

		Entity axis_y = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} }, {}, Colors::kGreen);
		relationeer.SetParent(axis_y, axis);

		Entity axis_z = CreateLine(world, { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }, {}, Colors::kBlue);
		relationeer.SetParent(axis_z, axis);
		
		return axis;
	}

	static Entity CreateLine(
		World& world, 
		std::vector<Vector3f> points, 
		Transform transform = {}, 
		Vector3f color = Colors::kGreen) 
	{
		Entity line = CreateTransformEntity(world, transform);
		world.Add<LineComponent>(line, color, std::move(points));

		return line;
	}

	static Entity CreateCircle(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen)
	{
		const float kRadius = 1.0f;
		std::vector<Vector3f> points = CreateCirclePoints(kRadius);

		Entity circle = CreateTransformEntity(world, transform);
		world.Add<LineComponent>(circle, color, std::move(points));

		return circle;
	}

	static Entity CreateSquare(
		World& world,
		Transform transform = {},
		Vector3f color = Colors::kGreen)
	{
		const float kSide = 1.0f;
		std::vector<Vector3f> points = CreateSquarePoints(kSide);

		Entity square = CreateTransformEntity(world, transform);
		world.Add<LineComponent>(square, color, std::move(points));

		return square;
	}

	static Entity CreateSphere(
		World& world, 
		Relationeer<TransformComponent>& relationeer, 
		Transform transform = {},
		Vector3f color = Colors::kGreen)
	{
		Entity sphere = CreateTransformEntity(world, transform);

		Transform roll_transform;
		roll_transform.rotation = Math::kQIdentity;
		
		Entity roll = CreateCircle(world, roll_transform);
		relationeer.SetParent(roll, sphere);

		Transform pitch_transform;
		pitch_transform.rotation = Quaternion::FromEulerAngles(0.0f, Math::kPiDiv2, 0.0f);
		
		Entity pitch = CreateCircle(world, pitch_transform);
		relationeer.SetParent(pitch, sphere);

		Transform yaw_transform;
		yaw_transform.rotation = Quaternion::FromEulerAngles(Math::kPiDiv2, 0.0f, 0.0f);
		
		Entity yaw = CreateCircle(world, yaw_transform);
		relationeer.SetParent(yaw, sphere);

		return sphere;
	}

	static Entity CreateCube(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Transform transform = {},
		Vector3f color = Colors::kGreen) 
	{
		const float kSide = 1.0f;
		const float kHalfSide = kSide / 2;
		
		Entity cube = CreateTransformEntity(world, transform);

		Transform front_transform;
		front_transform.position = { 0.0f, 0.0f, kHalfSide };
		front_transform.rotation = Math::kQIdentity;
		
		Entity front = CreateSquare(world, front_transform);
		relationeer.SetParent(front, cube);

		Transform top_transform;
		top_transform.position = { 0.0f, kHalfSide, 0.0f};
		top_transform.rotation = Quaternion::FromEulerAngles(Math::kPiDiv2, 0.0f, 0.0f);

		Entity top = CreateSquare(world, top_transform);
		relationeer.SetParent(top, cube);

		Transform back_transform;
		back_transform.position = { 0.0f, 0.0f, -kHalfSide };
		back_transform.rotation = Math::kQIdentity;

		Entity back = CreateSquare(world, back_transform);
		relationeer.SetParent(back, cube);

		Transform bottom_transform;
		bottom_transform.position = { 0.0f, -kHalfSide, 0.0f };
		bottom_transform.rotation = Quaternion::FromEulerAngles(Math::kPiDiv2, 0.0f, 0.0f);

		Entity bottom = CreateSquare(world, bottom_transform);
		relationeer.SetParent(bottom, cube);

		return cube;
	}

	static Entity CreateGrid(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Vector3i resolution,
		Transform transform = {},
		Vector3f color = Colors::kGreen) 
	{
		AOE_ASSERT_MSG(resolution.x >= 0, "Invalid resolution, resolution.x shouldn't be negative.");
		AOE_ASSERT_MSG(resolution.y >= 0, "Invalid resolution, resolution.y shouldn't be negative.");
		AOE_ASSERT_MSG(resolution.z >= 0, "Invalid resolution, resolution.z shouldn't be negative.");

		Entity grid = CreateTransformEntity(world, transform);

		Vector3f pivot{ 
			static_cast<float>(-resolution.x) / 2, 
			static_cast<float>(-resolution.y) / 2, 
			static_cast<float>(-resolution.z) / 2
		};

		for (int x = 0; x <= resolution.x; ++x) {
			for (int z = 0; z <= resolution.z; ++z) {
				Vector3f a = pivot + Vector3f(x, 0.0f, z);
				Vector3f b = a + Vector3f(0.0f, resolution.y, 0.0f);

				Entity line = CreateLine(world, { a, b }, {}, color);
				relationeer.SetParent(line, grid);
			}
		}

		for (int x = 0; x <= resolution.x; ++x) {
			for (int y = 0; y <= resolution.y; ++y) {
				Vector3f a = pivot + Vector3f(x, y, 0.0f);
				Vector3f b = a + Vector3f(0.0f, 0.0f, resolution.z);

				Entity line = CreateLine(world, { a, b }, {}, color);
				relationeer.SetParent(line, grid);
			}
		}

		for (int y = 0; y <= resolution.y; ++y) {
			for (int z = 0; z <= resolution.z; ++z) {
				Vector3f a = pivot + Vector3f(0.0f, y, z);
				Vector3f b = a + Vector3f(resolution.x, 0.0f, 0.0f);

				Entity line = CreateLine(world, { a, b }, {}, color);
				relationeer.SetParent(line, grid);
			}
		}

		return grid;
	}

private:
	static Entity CreateTransformEntity(World& world, Transform transform) {
		Entity entity = world.Create();
		world.Add<TransformComponent>(entity, transform);

		return entity;
	}

	static std::vector<Vector3f> CreateCirclePoints(float radius) {
		const int kStep = 10;
		const int kSize = 360 / kStep + 1;

		std::vector<Vector3f> points;
		points.reserve(kSize);

		for (int angle = 0; angle <= 360; angle += kStep) {
			float radians = Math::kDeg2Rad * angle;
			float x = Math::Sin(radians) * radius;
			float y = Math::Cos(radians) * radius;
			points.push_back({ x, y, 0.0f });
		}
		
		return points;
	}

	static std::vector<Vector3f> CreateSquarePoints(float side) {
		float half_side = side / 2;

		std::vector<Vector3f> points = {
			{ -half_side,  half_side, 0.0f },
			{ -half_side, -half_side, 0.0f },
			{  half_side, -half_side, 0.0f },
			{  half_side,  half_side, 0.0f },
			{ -half_side,  half_side, 0.0f },
		};

		return points;
	}
};

} // namespace aoe