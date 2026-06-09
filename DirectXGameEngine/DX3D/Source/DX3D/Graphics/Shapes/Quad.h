#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/Math/Vec3.h>
#include <DX3D/Math/Vec4.h>

namespace dx3d
{
	class Quad final : public Base
	{
	public:
		struct QuadDesc
		{
			BaseDesc base;
			GraphicsDevice& graphicsDevice;
			DeviceContext& deviceContext;
			GraphicsPipelineState& pipeline;
			Vec3 position{ 0.0f, 0.0f, 0.0f };
			Vec3 scale{ 1.0f, 1.0f, 1.0f };
			Vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
		};

		explicit Quad(const QuadDesc& desc);
		virtual ~Quad() override;

		void setPosition(const Vec3& position) noexcept;
		void setScale(const Vec3& scale) noexcept;
		void setColor(const Vec4& color) noexcept;

		const Vec3& getPosition() const noexcept;
		const Vec3& getScale() const noexcept;
		const Vec4& getColor() const noexcept;

		void render();

	private:
		struct Vertex
		{
			Vec3 position;
			Vec4 color;
		};

		void updateVertexBuffer();

	private:
		GraphicsDevice& m_graphicsDevice;
		DeviceContext& m_deviceContext;
		GraphicsPipelineState& m_pipeline;
		RefPtr<VertexBuffer> m_vb{};

		Vec3 m_position;
		Vec3 m_scale;
		Vec4 m_color;
		bool m_isDirty{ true };
	};
}