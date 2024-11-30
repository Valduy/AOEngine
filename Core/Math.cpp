#include "pch.h"

#include <cmath>

#include "Math.h"

namespace aoe {

const Vector2f Math::kZeros2f = mathfu::kZeros2f;
const Vector2f Math::kOnes2f = mathfu::kOnes2f;
const Vector2f Math::kAxisX2f = mathfu::kAxisX2f;
const Vector2f Math::kAxisY2f = mathfu::kAxisY2f;

const Vector3f Math::kZeros3f = mathfu::kZeros3f;
const Vector3f Math::kOnes3f = mathfu::kOnes3f;
const Vector3f Math::kAxisX3f = mathfu::kAxisX3f;
const Vector3f Math::kAxisY3f = mathfu::kAxisY3f;
const Vector3f Math::kAxisZ3f = mathfu::kAxisZ3f;
const Vector3f Math::kRight = kAxisX3f;
const Vector3f Math::kLeft = -kRight;
const Vector3f Math::kUp = kAxisY3f;
const Vector3f Math::kDown = -kUp;
const Vector3f Math::kForward = kAxisZ3f;
const Vector3f Math::kBackward = -kForward;

const Vector4f Math::kZeros4f = mathfu::kZeros4f;
const Vector4f Math::kOnes4f = mathfu::kOnes4f;
const Vector4f Math::kAxisX4f = mathfu::kAxisX4f;
const Vector4f Math::kAxisY4f = mathfu::kAxisY4f;
const Vector4f Math::kAxisZ4f = mathfu::kAxisZ4f;
const Vector4f Math::kAxisW4f = mathfu::kAxisW4f;

const Vector2i Math::kZeros2i = mathfu::kZeros2i;
const Vector2i Math::kOnes2i = mathfu::kOnes2i;
const Vector2i Math::kAxisX2i = mathfu::kAxisX2i;
const Vector2i Math::kAxisY2i = mathfu::kAxisY2i;

const Vector3i Math::kZeros3i = mathfu::kZeros3i;
const Vector3i Math::kOnes3i = mathfu::kOnes3i;
const Vector3i Math::kAxisX3i = mathfu::kAxisX3i;
const Vector3i Math::kAxisY3i = mathfu::kAxisY3i;
const Vector3i Math::kAxisZ3i = mathfu::kAxisZ3i;

const Vector4i Math::kZeros4i = mathfu::kZeros4i;
const Vector4i Math::kOnes4i = mathfu::kOnes4i;
const Vector4i Math::kAxisX4i = mathfu::kAxisX4i;
const Vector4i Math::kAxisY4i = mathfu::kAxisY4i;
const Vector4i Math::kAxisZ4i = mathfu::kAxisZ4i;
const Vector4i Math::kAxisW4i = mathfu::kAxisW4i;

const Quaternion Math::kQIdentity = mathfu::kQuatIdentityf;

bool Math::IsEqual(float lhs, float rhs, float epsilon) {
	return Abs(lhs - rhs) < epsilon;
}

bool Math::IsEqual(const Vector2f& lhs, const Vector2f& rhs, float epsilon) {
	return Math::IsEqual(lhs.x, rhs.x, epsilon) 
		&& Math::IsEqual(lhs.y, rhs.y, epsilon);
}

float Math::Pow(float value, float power) {
	return pow(value, power);
}

float Math::Min(float lhs, float rhs) {
	return lhs < rhs ? lhs : rhs;
}

float Math::Abs(float value) {
	return fabs(value);
}

float Math::Max(float lhs, float rhs) {
	return lhs > rhs ? lhs : rhs;
}

float Math::Sin(float angle) {
	return std::sin(angle);
}

float Math::Cos(float angle) {
	return std::cos(angle);
}

} // namespace aoe