#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Common.h>
#include <DX3D/Resource/Resource.h>
#include <DX3D/Graphics/Mesh/Mesh.h>

namespace dx3d
{
    class MeshResource : public Resource
    {
    public:
        explicit MeshResource(const MeshResourceDesc& desc);

        RefPtr<Mesh> getMesh() const { return m_mesh; }

    private:
        RefPtr<Mesh> m_mesh;
        GraphicsDevice& m_graphicsDevice;
    };
}
