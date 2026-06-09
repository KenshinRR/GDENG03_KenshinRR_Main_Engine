#include <DX3D/Graphics/Shapes/Quad.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/DeviceContext.h>
#include <DX3D/Graphics/VertexBuffer.h>
#include <DX3D/Graphics/GraphicsPipelineState.h>

using namespace dx3d;

dx3d::Quad::Quad(const QuadDesc& desc)
	: Base(desc.base)
	, m_graphicsDevice(desc.graphicsDevice)
	, m_deviceContext(desc.deviceContext)
	, m_pipeline(desc.pipeline)
	, m_position(desc.position)
	, m_scale(desc.scale)
	, m_color(desc.color)
{
	updateVertexBuffer();
}

dx3d::Quad::~Quad()
{
}

void dx3d::Quad::setPosition(const Vec3& position) noexcept
{
	m_position = position;
	m_isDirty = true;
}

void dx3d::Quad::setScale(const Vec3& scale) noexcept
{
	m_scale = scale;
	m_isDirty = true;
}

void dx3d::Quad::setColor(const Vec4& color) noexcept
{
	m_color = color;
	m_isDirty = true;
}

const Vec3& dx3d::Quad::getPosition() const noexcept
{
	return m_position;
}

const Vec3& dx3d::Quad::getScale() const noexcept
{
	return m_scale;
}

const Vec4& dx3d::Quad::getColor() const noexcept
{
	return m_color;
}

void dx3d::Quad::updateVertexBuffer()
{
	f32 halfWidth = m_scale.x * 0.5f;
	f32 halfHeight = m_scale.y * 0.5f;

	// Define quad vertices (2 triangles)
	const Vertex vertexList[] = {
		// First triangle
		{ m_position + Vec3(-halfWidth, halfHeight, 0.0f), m_color },   // Top-left
		{ m_position + Vec3(halfWidth, halfHeight, 0.0f), m_color },    // Top-right
		{ m_position + Vec3(halfWidth, -halfHeight, 0.0f), m_color },   // Bottom-right

		// Second triangle
		{ m_position + Vec3(halfWidth, -halfHeight, 0.0f), m_color },   // Bottom-right
		{ m_position + Vec3(-halfWidth, -halfHeight, 0.0f), m_color },  // Bottom-left
		{ m_position + Vec3(-halfWidth, halfHeight, 0.0f), m_color },   // Top-left
	};

	m_vb = m_graphicsDevice.createVertexBuffer({ vertexList, std::size(vertexList), sizeof(Vertex) });
	m_isDirty = false;
}

void dx3d::Quad::render()
{
	if (m_isDirty)
	{
		updateVertexBuffer();
	}

	m_deviceContext.SetGraphicsPipelineState(m_pipeline);
	m_deviceContext.setVertexBuffer(*m_vb);
	m_deviceContext.drawTriangleList(m_vb->getVertexListsize(), 0u);
}