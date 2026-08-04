#pragma once
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Mat4x4.h>
#include <cmath>

namespace dx3d
{
    class Quaternion
    {
    public:
        float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

        Quaternion() = default;
        Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

        static Quaternion fromRotationMatrix(const Mat4x4& m)
        {
            // Extract the 3x3 rotation portion from the matrix
            // Note: Your matrix is m_data[row][col]
            float m00 = m.m_data[0][0], m01 = m.m_data[0][1], m02 = m.m_data[0][2];
            float m10 = m.m_data[1][0], m11 = m.m_data[1][1], m12 = m.m_data[1][2];
            float m20 = m.m_data[2][0], m21 = m.m_data[2][1], m22 = m.m_data[2][2];

            float trace = m00 + m11 + m22;
            Quaternion q;

            if (trace > 0.0f)
            {
                float s = 0.5f / std::sqrt(trace + 1.0f);
                q.w = 0.25f / s;
                q.x = (m21 - m12) * s;
                q.y = (m02 - m20) * s;
                q.z = (m10 - m01) * s;
            }
            else if (m00 > m11 && m00 > m22)
            {
                float s = 2.0f * std::sqrt(1.0f + m00 - m11 - m22);
                q.w = (m21 - m12) / s;
                q.x = 0.25f * s;
                q.y = (m01 + m10) / s;
                q.z = (m02 + m20) / s;
            }
            else if (m11 > m22)
            {
                float s = 2.0f * std::sqrt(1.0f + m11 - m00 - m22);
                q.w = (m02 - m20) / s;
                q.x = (m01 + m10) / s;
                q.y = 0.25f * s;
                q.z = (m12 + m21) / s;
            }
            else
            {
                float s = 2.0f * std::sqrt(1.0f + m22 - m00 - m11);
                q.w = (m10 - m01) / s;
                q.x = (m02 + m20) / s;
                q.y = (m12 + m21) / s;
                q.z = 0.25f * s;
            }

            // Normalize
            float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
            if (len > 0.00001f)
            {
                q.x /= len;
                q.y /= len;
                q.z /= len;
                q.w /= len;
            }

            return q;
        }

        Vec3 toEulerAngles() const
        {
            Vec3 euler;

            // Roll (x-axis rotation)
            float sinr_cosp = 2.0f * (w * x + y * z);
            float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
            euler.x = std::atan2(sinr_cosp, cosr_cosp);

            // Pitch (y-axis rotation)
            float sinp = 2.0f * (w * y - z * x);
            if (std::abs(sinp) >= 1.0f)
                euler.y = std::copysign(3.14159265359f / 2.0f, sinp);
            else
                euler.y = std::asin(sinp);

            // Yaw (z-axis rotation)
            float siny_cosp = 2.0f * (w * z + x * y);
            float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
            euler.z = std::atan2(siny_cosp, cosy_cosp);

            return euler;
        }
    };
}
