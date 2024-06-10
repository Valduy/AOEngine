#include "pch.h"

#include "DebugUtils.h"

namespace aoe {

Entity DebugUtils::CreateAxis(
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

Entity DebugUtils::CreateLine(
	World& world,
	std::vector<Vector3f> points,
	Transform transform,
	Vector3f color)
{
	Entity entity = CreateTransformEntity(world, transform);

	Line line({ Segment(std::move(points)), });
	world.Add<LineComponent>(entity, color, line);

	return entity;
}

Entity DebugUtils::CreateCircle(
	World& world,
	Transform transform,
	Vector3f color)
{
	std::vector<Vector3f> points = CreateCirclePoints(kUnit / 2);
	return CreateLine(world, points, transform, color);
}

Entity DebugUtils::CreateSquare(
	World& world,
	Transform transform,
	Vector3f color)
{
	std::vector<Vector3f> points = CreateSquarePoints(kUnit);
	return CreateLine(world, points, transform, color);
}

Entity DebugUtils::CreateSphere(
	World& world,
	Transform transform,
	Vector3f color)
{
	const float kRadius = kUnit / 2;

	Entity entity = CreateTransformEntity(world, transform);

	Segment roll = CreateCirclePoints(kRadius);
	Segment pitch = CreateCirclePoints(kRadius, Math::kZeros3f, { 0.0f, Math::kPiDiv2, 0.0f });
	Segment yaw = CreateCirclePoints(kRadius, Math::kZeros3f, { Math::kPiDiv2, 0.0f, 0.0f });

	Line line({ roll, pitch, yaw });
	world.Add<LineComponent>(entity, color, std::move(line));

	return entity;
}

Entity DebugUtils::CreateCube(
	World& world,
	Transform transform,
	Vector3f color)
{
	const float kHalfSide = kUnit / 2;

	Entity entity = CreateTransformEntity(world, transform);

	Segment front = CreateSquarePoints(kUnit, { 0.0f, 0.0f, kHalfSide });
	Segment back = CreateSquarePoints(kUnit, { 0.0f, 0.0f, -kHalfSide });

	Segment segment0 = Segment({ 
		{ -kHalfSide, kHalfSide, -kHalfSide, }, 
		{ -kHalfSide, kHalfSide,  kHalfSide, },
	});

	Segment segment1 = Segment({
		{ -kHalfSide, -kHalfSide, -kHalfSide, },
		{ -kHalfSide, -kHalfSide,  kHalfSide, },
	});

	Segment segment2 = Segment({
		{ kHalfSide, -kHalfSide, -kHalfSide, },
		{ kHalfSide, -kHalfSide,  kHalfSide, },
	});

	Segment segment3 = Segment({
		{ kHalfSide, kHalfSide, -kHalfSide, },
		{ kHalfSide, kHalfSide,  kHalfSide, },
	});

	Line line({ front, back, segment0, segment1, segment2, segment3, });
	world.Add<LineComponent>(entity, color, std::move(line));

	return entity;
}

Entity DebugUtils::CreateGrid(
	World& world,
	Vector3i resolution,
	Transform transform,
	Vector3f color)
{
	AOE_ASSERT_MSG(resolution.x >= 0, "Invalid resolution, resolution.x shouldn't be negative.");
	AOE_ASSERT_MSG(resolution.y >= 0, "Invalid resolution, resolution.y shouldn't be negative.");
	AOE_ASSERT_MSG(resolution.z >= 0, "Invalid resolution, resolution.z shouldn't be negative.");

	Entity entity = CreateTransformEntity(world, transform);

	std::vector<Segment> segments;
	
	Vector3f pivot{
		static_cast<float>(-resolution.x) / 2,
		static_cast<float>(-resolution.y) / 2,
		static_cast<float>(-resolution.z) / 2
	};

	if (resolution.z > 0) {
		for (int x = 0; x <= resolution.x; ++x) {
			for (int y = 0; y <= resolution.y; ++y) {
				Vector3f a = pivot + Vector3f(x, y, 0.0f);
				Vector3f b = a + Vector3f(0.0f, 0.0, resolution.z);
				
				std::vector<Vector3f> points = { a, b };
				segments.emplace_back(std::move(points));
			}
		}
	}

	if (resolution.y > 0) {
		for (int x = 0; x <= resolution.x; ++x) {
			for (int z = 0; z <= resolution.z; ++z) {
				Vector3f a = pivot + Vector3f(x, 0.0f, z);
				Vector3f b = a + Vector3f(0.0f, resolution.y, 0.0f);
				
				std::vector<Vector3f> points = { a, b };
				segments.emplace_back(std::move(points));
			}
		}
	}

	if (resolution.x > 0) {
		for (int y = 0; y <= resolution.y; ++y) {
			for (int z = 0; z <= resolution.z; ++z) {
				Vector3f a = pivot + Vector3f(0.0f, y, z);
				Vector3f b = a + Vector3f(resolution.x, 0.0f, 0.0f);
				
				std::vector<Vector3f> points = { a, b };
				segments.emplace_back(std::move(points));
			}
		}
	}

	Line line(segments);
	world.Add<LineComponent>(entity, color, std::move(line));

	return entity;
}

Entity DebugUtils::CreateTransformEntity(World& world, Transform transform) {
	Entity entity = world.Create();
	world.Add<TransformComponent>(entity, transform);

	return entity;
}

std::vector<Vector3f> DebugUtils::CreateCirclePoints(float radius, Vector3f offset, Vector3f angles) {
	const int kStep = 10;
	const int kSize = 360 / kStep + 1;

	Matrix4f transformation = Matrix4f::Identity();
	transformation *= Matrix4f::FromTranslationVector(offset);
	transformation *= Quaternion::FromEulerAngles(angles).ToMatrix4();

	std::vector<Vector3f> points;
	points.reserve(kSize);

	for (int angle = 0; angle <= 360; angle += kStep) {
		float radians = Math::kDeg2Rad * angle;
		float x = Math::Sin(radians) * radius;
		float y = Math::Cos(radians) * radius;

		Vector3f point = Transformate({ x, y, 0.0f }, transformation);
		points.push_back(point);
	}

	return points;
}

std::vector<Vector3f> DebugUtils::CreateSquarePoints(float side, Vector3f offset, Vector3f angles) {
	Matrix4f transformation = Matrix4f::Identity();
	transformation *= Matrix4f::FromTranslationVector(offset);
	transformation *= Quaternion::FromEulerAngles(angles).ToMatrix4();

	float half_side = side / 2;

	std::vector<Vector3f> points = {
		Transformate({-half_side,  half_side, 0.0f}, transformation),
		Transformate({-half_side, -half_side, 0.0f}, transformation),
		Transformate({ half_side, -half_side, 0.0f}, transformation),
		Transformate({ half_side,  half_side, 0.0f}, transformation),
		Transformate({-half_side,  half_side, 0.0f}, transformation),
	};

	return points;
}

Vector3f DebugUtils::Transformate(const Vector3f& point, const Matrix4f transformation) {
	Vector4f transformed = transformation * Vector4f(point, 1.0f);
	return transformed.xyz();
}

} // namespace aoe