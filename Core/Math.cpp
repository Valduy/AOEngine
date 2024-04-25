#include "pch.h"

#include "Math.h"

namespace aoe {

const Vector2 Math::kV2Zero = mathfu::kZeros2f;
const Vector2 Math::kV2Ones = mathfu::kOnes2f;
const Vector2 Math::kV2AxisX = mathfu::kAxisX2f;
const Vector2 Math::kV2AxisY = mathfu::kAxisY2f;

const Vector3 Math::kV3Zero = mathfu::kZeros3f;
const Vector3 Math::kV3Ones = mathfu::kOnes3f;
const Vector3 Math::kV3AxisX = mathfu::kAxisX3f;
const Vector3 Math::kV3AxisY = mathfu::kAxisY3f;
const Vector3 Math::kV3AxisZ = mathfu::kAxisZ3f;
const Vector3 Math::kRight = kV3AxisX;
const Vector3 Math::kLeft = -kRight;
const Vector3 Math::kUp = kV3AxisY;
const Vector3 Math::kDown = -kUp;
const Vector3 Math::kForward = kV3AxisZ;
const Vector3 Math::kBackward = -kForward;

const Vector4 Math::kV4Zero = mathfu::kZeros4f;
const Vector4 Math::kV4Ones = mathfu::kOnes4f;
const Vector4 Math::kV4AxisX = mathfu::kAxisX4f;
const Vector4 Math::kV4AxisY = mathfu::kAxisY4f;
const Vector4 Math::kV4AxisZ = mathfu::kAxisZ4f;
const Vector4 Math::kV4AxisW = mathfu::kAxisW4f;

const Quaternion Math::kQIdentity = mathfu::kQuatIdentityf;


} // namespace aoe