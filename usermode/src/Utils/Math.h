#pragma once

#include <cmath>

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;

	Vector3 operator-(const Vector3& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}

	float Length() {
		return sqrtf(x * x + y * y + z * z);
	}
};

struct Matrix4 {
	float m[4][4];
};

inline bool WorldToScreen(const Vector3& worldPos, Vector2& screenPos, const Matrix4& viewMatrix, int screenW, int screenH) {

    float w = viewMatrix.m[3][0] * worldPos.x + viewMatrix.m[3][1] * worldPos.y + viewMatrix.m[3][2] * worldPos.z + viewMatrix.m[3][3];

    if (w < 0.001f)          // behind camera
        return false;

    float x = viewMatrix.m[0][0] * worldPos.x + viewMatrix.m[0][1] * worldPos.y + viewMatrix.m[0][2] * worldPos.z + viewMatrix.m[0][3];
    float y = viewMatrix.m[1][0] * worldPos.x + viewMatrix.m[1][1] * worldPos.y + viewMatrix.m[1][2] * worldPos.z + viewMatrix.m[1][3];

    screenPos.x = (screenW / 2.f) + (screenW / 2.f) * x / w;
    screenPos.y = (screenH / 2.f) - (screenH / 2.f) * y / w;

    return true;
}

inline float GetDistance(const Vector3& a, const Vector3& b) {
	return (a - b).Length() / 39.37f; // units to metres (apex uses inches internally)
}