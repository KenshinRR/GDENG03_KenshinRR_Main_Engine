#pragma once
#include <DX3D/Graphics/Mesh/Mesh.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Resource/Resource.h>

namespace dx3d
{
	class MeshFactory final : public Base
	{
	public:
		explicit MeshFactory(const ResourceManagerDesc& desc);

		RefPtr<Mesh> createCubeMesh();
		RefPtr<Mesh> createSphereMesh(ui32 stacks = 20, ui32 slices = 20);
		RefPtr<Mesh> createCapsuleMesh(f32 radius = 0.5f, f32 height = 2.0f, ui32 segments = 16, ui32 rings = 8);
		RefPtr<Mesh> createCylinderMesh(f32 radius = 0.5f, f32 height = 2.0f, ui32 segments = 32);
		RefPtr<Mesh> createPlaneMesh(f32 width = 1.0f, f32 height = 1.0f, ui32 widthSegments = 1, ui32 heightSegments = 1);
		RefPtr<Mesh> createCircleMesh(f32 radius = 0.5f, ui32 segments = 32);

	private:
		SystemContext m_context;
	};
}
