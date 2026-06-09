#pragma once
#include <DX3D/Core/Core.h>

namespace dx3d
{
	class Vec3
	{
	public:
		Vec3() = default;
		Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

		// Operator overloads
		Vec3 operator+(const Vec3& other) const noexcept
		{
			return Vec3(x + other.x, y + other.y, z + other.z);
		}

		Vec3 operator-(const Vec3& other) const noexcept
		{
			return Vec3(x - other.x, y - other.y, z - other.z);
		}

		Vec3 operator*(f32 scalar) const noexcept
		{
			return Vec3(x * scalar, y * scalar, z * scalar);
		}

		Vec3 operator/(f32 scalar) const noexcept
		{
			return Vec3(x / scalar, y / scalar, z / scalar);
		}

		Vec3& operator+=(const Vec3& other) noexcept
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return *this;
		}

	public:
		f32 x{}, y{}, z{};
	};

	// Free function for scalar * Vec3 (for cases like 2.0f * vec)
	inline Vec3 operator*(f32 scalar, const Vec3& vec) noexcept
	{
		return vec * scalar;
	}
}