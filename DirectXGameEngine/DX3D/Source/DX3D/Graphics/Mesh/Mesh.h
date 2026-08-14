#pragma once
#include <DX3D/Core/Core.h>
#include <DX3D/Core/Common.h>
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>
#include <vector>
#include <DX3D/Graphics/GraphicsResource.h>

namespace dx3d
{
	class Mesh : public GraphicsResource
	{
	public:
		Mesh(const MeshDesc& desc, const GraphicsResourceDesc& gDesc);

		const Vertex* getVertices() const noexcept;
		ui32 getVertexCount() const noexcept;

		const ui32* getIndices() const noexcept;
		ui32 getIndexCount() const noexcept;

		std::string getType() { return m_type; }
	private:
		std::vector<Vertex> m_vertices;
		std::vector<ui32> m_indices;

		std::string m_type;
	};
}
