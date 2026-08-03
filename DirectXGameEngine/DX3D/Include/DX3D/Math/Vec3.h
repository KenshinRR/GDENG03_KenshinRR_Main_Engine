#pragma once
#include <DX3D/Core/Core.h>
#include <cmath>

namespace dx3d
{
	class Vec3
	{
		public:
			Vec3() = default;
			Vec3(f32 x, f32 y, f32 z) : x(x), y(y), z(z) {}

			float length() const
			{
				return std::sqrt(x * x + y * y + z * z);
			}

			Vec3& operator+=(const Vec3& rhs)
			{
				x += rhs.x;
				y += rhs.y;
				z += rhs.z;
				return *this;
			}

			Vec3& operator-=(const Vec3& rhs)
			{
				x -= rhs.x;
				y -= rhs.y;
				z -= rhs.z;
				return *this;
			}

			Vec3& operator*=(float scalar)
			{
				x *= scalar;
				y *= scalar;
				z *= scalar;
				return *this;
			}

			Vec3& operator/=(float scalar)
			{
				if (scalar != 0.0f)
				{
					x /= scalar;
					y /= scalar;
					z /= scalar;
				}
				return *this;
			}

			static Vec3 normalize(const Vec3& v)
			{
				float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;

				if (lenSq == 0.0f) return Vec3{ 0.0f, 0.0f, 0.0f };

				float invLen = 1.0f / std::sqrt(lenSq);

				return Vec3{
					v.x * invLen,
					v.y * invLen,
					v.z * invLen
				};
			}

			static float dot(const Vec3& x, const Vec3& y) noexcept
			{
				return x.x * y.x + x.y * y.y + x.z * y.z;
			}

			static Vec3 cross(const Vec3& x, const Vec3& y) noexcept
			{
				return Vec3{
					x.y * y.z - x.z * y.y,
					x.z * y.x - x.x * y.z,
					x.x * y.y - x.y * y.x
				};
			}

		public:
			f32 x{}, y{}, z{};
		};

		inline Vec3 operator+(Vec3 lhs, const Vec3& rhs)
		{
			lhs += rhs;
			return lhs;
		}

		inline Vec3 operator-(Vec3 lhs, const Vec3& rhs)
		{
			lhs -= rhs;
			return lhs;
		}

		inline Vec3 operator-(const Vec3& v)
		{
			return Vec3{ -v.x, -v.y, -v.z };
		}

		inline Vec3 operator*(Vec3 v, float scalar)
		{
			v *= scalar;
			return v;
		}

		inline Vec3 operator*(float scalar, Vec3 v)
		{
			v *= scalar;
			return v;
		}

		inline Vec3 operator/(Vec3 v, float scalar)
		{
			v /= scalar;
			return v;
		}
}