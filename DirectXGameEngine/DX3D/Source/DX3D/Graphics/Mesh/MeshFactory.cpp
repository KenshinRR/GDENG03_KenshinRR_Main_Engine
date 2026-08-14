#include <DX3D/Graphics/Mesh/MeshFactory.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Resource/MeshResource.h>
#include <DX3D/Graphics/Mesh/Mesh.h>
#include <cmath>

dx3d::MeshFactory::MeshFactory(const ResourceManagerDesc& desc)
    : Base(desc.base), m_context(desc.context)
{
}

dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createCubeMesh()
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    // Each face has its own vertices so normals/UVs are correct
    auto addFace = [&](Vec3 normal, Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) {
        ui32 start = static_cast<ui32>(vertices.size());
        vertices.push_back({ v0, {0.0f,1.0f}, normal });
        vertices.push_back({ v1, {0.0f,0.0f}, normal });
        vertices.push_back({ v2, {1.0f,0.0f}, normal });
        vertices.push_back({ v3, {1.0f,1.0f}, normal });
        indices.insert(indices.end(), { start, start + 1, start + 2, start, start + 2, start + 3 });
        };

    // Front
    addFace({ 0,0,-1 }, { -0.5f,-0.5f,-0.5f }, { -0.5f,0.5f,-0.5f }, { 0.5f,0.5f,-0.5f }, { 0.5f,-0.5f,-0.5f });
    // Back
    addFace({ 0,0,1 }, { 0.5f,-0.5f,0.5f }, { 0.5f,0.5f,0.5f }, { -0.5f,0.5f,0.5f }, { -0.5f,-0.5f,0.5f });
    // Left
    addFace({ -1,0,0 }, { -0.5f,-0.5f,0.5f }, { -0.5f,0.5f,0.5f }, { -0.5f,0.5f,-0.5f }, { -0.5f,-0.5f,-0.5f });
    // Right
    addFace({ 1,0,0 }, { 0.5f,-0.5f,-0.5f }, { 0.5f,0.5f,-0.5f }, { 0.5f,0.5f,0.5f }, { 0.5f,-0.5f,0.5f });
    // Top
    addFace({ 0,1,0 }, { -0.5f,0.5f,-0.5f }, { -0.5f,0.5f,0.5f }, { 0.5f,0.5f,0.5f }, { 0.5f,0.5f,-0.5f });
    // Bottom
    addFace({ 0,-1,0 }, { -0.5f,-0.5f,0.5f }, { -0.5f,-0.5f,-0.5f }, { 0.5f,-0.5f,-0.5f }, { 0.5f,-0.5f,0.5f });

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}

dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createSphereMesh(ui32 stacks, ui32 slices)
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    constexpr f32 PI = 3.14159265359f;
    constexpr f32 TWO_PI = 2.0f * PI;

    for (ui32 i = 0; i <= stacks; ++i)
    {
        f32 stackAngle = PI / 2.0f - (i * PI / stacks);
        f32 xy = 0.5f * cosf(stackAngle);
        f32 z = 0.5f * sinf(stackAngle);

        for (ui32 j = 0; j <= slices; ++j)
        {
            f32 sliceAngle = (j * TWO_PI / slices);
            f32 x = xy * cosf(sliceAngle);
            f32 y = xy * sinf(sliceAngle);

            Vec3 pos = { x, y, z };
            Vec3 normal = normal.normalize(pos);
            Vec2 uv = { static_cast<f32>(j) / slices, static_cast<f32>(i) / stacks };

            vertices.push_back({ pos, uv, normal });
        }
    }

    for (ui32 i = 0; i < stacks; ++i)
    {
        ui32 k1 = i * (slices + 1);
        ui32 k2 = k1 + slices + 1;

        for (ui32 j = 0; j < slices; ++j)
        {
            if (i != 0)
            {
                indices.insert(indices.end(), { k1, k2, k1 + 1 });
            }
            if (i != (stacks - 1))
            {
                indices.insert(indices.end(), { k1 + 1, k2, k2 + 1 });
            }
            k1++; k2++;
        }
    }

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}

dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createCapsuleMesh(f32 radius, f32 height, ui32 segments, ui32 rings)
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    constexpr f32 PI = 3.14159265359f;
    constexpr f32 TWO_PI = 2.0f * PI;
    f32 halfHeight = height / 2.0f;

    // Top hemisphere
    for (ui32 i = 0; i <= rings; ++i)
    {
        f32 phi = (PI / 2.0f) * (i / static_cast<f32>(rings));
        f32 z_offset = halfHeight + radius * sinf(phi);

        for (ui32 j = 0; j <= segments; ++j)
        {
            f32 theta = TWO_PI * (j / static_cast<f32>(segments));
            f32 r = radius * cosf(phi);
            f32 x = r * cosf(theta);
            f32 y = r * sinf(theta);

            Vec3 pos = { x, y, z_offset };
            Vec3 normal = normal.normalize(pos);
            Vec2 uv = { static_cast<f32>(j) / segments, static_cast<f32>(i) / rings };

            vertices.push_back({ pos, uv, normal });
        }
    }

    // Cylinder body
    for (ui32 i = 0; i <= 1; ++i)
    {
        f32 z_offset = (i == 0 ? halfHeight : -halfHeight);
        for (ui32 j = 0; j <= segments; ++j)
        {
            f32 theta = TWO_PI * (j / static_cast<f32>(segments));
            f32 x = radius * cosf(theta);
            f32 y = radius * sinf(theta);

            Vec3 pos = { x, y, z_offset };
            Vec3 normal = normal.normalize({ x, y, 0.0f });
            Vec2 uv = { static_cast<f32>(j) / segments, i == 0 ? 0.0f : 1.0f };

            vertices.push_back({ pos, uv, normal });
        }
    }

    // Bottom hemisphere
    for (ui32 i = 1; i <= rings; ++i)
    {
        f32 phi = -(PI / 2.0f) * (i / static_cast<f32>(rings));
        f32 z_offset = -halfHeight + radius * sinf(phi);

        for (ui32 j = 0; j <= segments; ++j)
        {
            f32 theta = TWO_PI * (j / static_cast<f32>(segments));
            f32 r = radius * cosf(phi);
            f32 x = r * cosf(theta);
            f32 y = r * sinf(theta);

            Vec3 pos = { x, y, z_offset };
            Vec3 normal = normal.normalize(pos);
            Vec2 uv = { static_cast<f32>(j) / segments, static_cast<f32>(i) / rings };

            vertices.push_back({ pos, uv, normal });
        }
    }

    // Indices for cylinder
    ui32 cylStart = (rings + 1) * (segments + 1);
    for (ui32 j = 0; j < segments; ++j)
    {
        ui32 k1 = cylStart + j;
        ui32 k2 = k1 + (segments + 1);

        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);

        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
    }

    // Indices for top hemisphere (reversed winding)
    for (ui32 i = 0; i < rings; ++i)
    {
        ui32 k1 = i * (segments + 1);
        ui32 k2 = k1 + segments + 1;

        for (ui32 j = 0; j < segments; ++j)
        {
            indices.push_back(k1);
            indices.push_back(k1 + 1);
            indices.push_back(k2);

            indices.push_back(k1 + 1);
            indices.push_back(k2 + 1);
            indices.push_back(k2);

            k1++;
            k2++;
        }
    }

    // Indices for bottom hemisphere
    ui32 bottomStart = cylStart + 2 * (segments + 1);
    for (ui32 i = 0; i < rings; ++i)
    {
        ui32 k1 = bottomStart + i * (segments + 1);
        ui32 k2 = k1 + segments + 1;

        for (ui32 j = 0; j < segments; ++j)
        {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);

            k1++;
            k2++;
        }
    }

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}

dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createCylinderMesh(f32 radius, f32 height, ui32 segments)
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    constexpr f32 TWO_PI = 6.28318530718f;
    f32 halfHeight = height / 2.0f;

    // Top center
    vertices.push_back({ {0.0f, halfHeight, 0.0f}, {0.5f,0.0f}, {0.0f,1.0f,0.0f} });
    for (ui32 i = 0; i < segments; ++i)
    {
        f32 theta = TWO_PI * (i / static_cast<f32>(segments));
        f32 x = radius * cosf(theta);
        f32 z = radius * sinf(theta);

        Vec2 uv = { (cosf(theta) * 0.5f + 0.5f), (sinf(theta) * 0.5f + 0.5f) };
        vertices.push_back({ {x, halfHeight, z}, uv, {0.0f,1.0f,0.0f} });
    }

    // Bottom center
    ui32 bottomCenterIdx = static_cast<ui32>(vertices.size());
    vertices.push_back({ {0.0f, -halfHeight, 0.0f}, {0.5f,1.0f}, {0.0f,-1.0f,0.0f} });
    for (ui32 i = 0; i < segments; ++i)
    {
        f32 theta = TWO_PI * (i / static_cast<f32>(segments));
        f32 x = radius * cosf(theta);
        f32 z = radius * sinf(theta);

        Vec2 uv = { (cosf(theta) * 0.5f + 0.5f), (sinf(theta) * 0.5f + 0.5f) };
        vertices.push_back({ {x, -halfHeight, z}, uv, {0.0f,-1.0f,0.0f} });
    }

    // Side vertices
    ui32 sideStartIdx = static_cast<ui32>(vertices.size());
    for (ui32 i = 0; i < segments; ++i)
    {
        f32 theta = TWO_PI * (i / static_cast<f32>(segments));
        f32 x = radius * cosf(theta);
        f32 z = radius * sinf(theta);

        Vec3 normal = normal.normalize({ x,0.0f,z });
        Vec2 uvTop = { static_cast<f32>(i) / segments, 0.0f };
        Vec2 uvBottom = { static_cast<f32>(i) / segments, 1.0f };

        vertices.push_back({ {x, halfHeight, z}, uvTop, normal });
        vertices.push_back({ {x, -halfHeight, z}, uvBottom, normal });
    }

    // Top cap (outward)
    for (ui32 i = 1; i < segments; ++i)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(segments);
    indices.push_back(1);

    // Top cap (inward, reversed winding)
    for (ui32 i = 1; i < segments; ++i)
    {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(segments);

    // Bottom cap (outward)
    for (ui32 i = 1; i < segments; ++i)
    {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomCenterIdx + i);
        indices.push_back(bottomCenterIdx + i + 1);
    }
    indices.push_back(bottomCenterIdx);
    indices.push_back(bottomCenterIdx + segments);
    indices.push_back(bottomCenterIdx + 1);

    // Bottom cap (inward, reversed winding)
    for (ui32 i = 1; i < segments; ++i)
    {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomCenterIdx + i + 1);
        indices.push_back(bottomCenterIdx + i);
    }
    indices.push_back(bottomCenterIdx);
    indices.push_back(bottomCenterIdx + 1);
    indices.push_back(bottomCenterIdx + segments);

    // Side faces
    for (ui32 i = 0; i < segments; ++i)
    {
        ui32 top1 = sideStartIdx + i * 2;
        ui32 bottom1 = top1 + 1;
        ui32 top2 = sideStartIdx + ((i + 1) % segments) * 2;
        ui32 bottom2 = top2 + 1;

        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(top2);

        indices.push_back(top2);
        indices.push_back(bottom1);
        indices.push_back(bottom2);
    }

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}


dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createPlaneMesh(f32 width, f32 height, ui32 widthSegments, ui32 heightSegments)
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    f32 halfWidth = width / 2.0f;
    f32 halfHeight = height / 2.0f;
    f32 widthStep = width / static_cast<f32>(widthSegments);
    f32 heightStep = height / static_cast<f32>(heightSegments);

    for (ui32 y = 0; y <= heightSegments; ++y)
    {
        for (ui32 x = 0; x <= widthSegments; ++x)
        {
            f32 posX = -halfWidth + (x * widthStep);
            f32 posY = 0.0f;
            f32 posZ = -halfHeight + (y * heightStep);

            Vec2 uv = { static_cast<f32>(x) / widthSegments, static_cast<f32>(y) / heightSegments };
            Vec3 normal = { 0.0f, 1.0f, 0.0f };

            vertices.push_back({ {posX, posY, posZ}, uv, normal });
        }
    }

    for (ui32 y = 0; y < heightSegments; ++y)
    {
        for (ui32 x = 0; x < widthSegments; ++x)
        {
            ui32 a = y * (widthSegments + 1) + x;
            ui32 b = a + 1;
            ui32 c = a + (widthSegments + 1);
            ui32 d = c + 1;

            indices.insert(indices.end(), { a,c,b, b,c,d });
        }
    }

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}

dx3d::RefPtr<dx3d::Mesh> dx3d::MeshFactory::createCircleMesh(f32 radius, ui32 segments)
{
    std::vector<Vertex> vertices;
    std::vector<ui32> indices;

    constexpr f32 TWO_PI = 6.28318530718f;

    vertices.push_back({ {0.0f,0.0f,0.0f}, {0.5f,0.5f}, {0.0f,1.0f,0.0f} });

    for (ui32 i = 0; i <= segments; ++i)
    {
        f32 theta = TWO_PI * (i / static_cast<f32>(segments));
        f32 x = radius * cosf(theta);
        f32 z = radius * sinf(theta);

        Vec2 uv = { (x / radius) * 0.5f + 0.5f, (z / radius) * 0.5f + 0.5f };
        vertices.push_back({ {x,0.0f,z}, uv, {0.0f,1.0f,0.0f} });
    }

    for (ui32 i = 1; i <= segments; ++i)
    {
        indices.insert(indices.end(), { 0,i,i + 1 });
    }

    return m_context.graphicsDevice.createMesh({ vertices, indices });
}
