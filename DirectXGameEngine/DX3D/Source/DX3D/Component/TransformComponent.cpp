#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Game/World.h>
#include <DX3D/Game/GameObject.h>

dx3d::TransformComponent::TransformComponent(const ComponentDesc& data) : Component(data)
{
	markAsDirty();
}

void dx3d::TransformComponent::setPosition(const Vec3& position)
{
	m_position = position;
	markAsDirty();
}

dx3d::Vec3 dx3d::TransformComponent::getPosition() const noexcept
{
	return m_position;
}

void dx3d::TransformComponent::setRotation(const Vec3& rotation)
{
	m_rotation = rotation;
	markAsDirty();
}

dx3d::Vec3 dx3d::TransformComponent::getRotation() const noexcept
{
	return m_rotation;
}

void dx3d::TransformComponent::setScale(const Vec3& scale)
{
	m_scale = scale;
	markAsDirty();
}

dx3d::Vec3 dx3d::TransformComponent::getScale() const noexcept
{
	return m_scale;
}

dx3d::Vec3 dx3d::TransformComponent::forward()
{
	auto forward = getRigidWorldMatrix().row(2);
	return dx3d::Vec3::normalize({ forward.x,forward.y,forward.z});
}

dx3d::Vec3 dx3d::TransformComponent::right()
{
	auto right = getRigidWorldMatrix().row(0);
	return dx3d::Vec3::normalize({ right.x,right.y,right.z });
}

dx3d::Vec3 dx3d::TransformComponent::up()
{
	auto up = getRigidWorldMatrix().row(1);
	return dx3d::Vec3::normalize({ up.x,up.y,up.z });
}

dx3d::Mat4x4 dx3d::TransformComponent::getAffineWorldMatrix() noexcept
{
	updateWorldMatrix();
	return m_affineWorldMatrix;
}

dx3d::Mat4x4 dx3d::TransformComponent::getRigidWorldMatrix() noexcept
{
	updateWorldMatrix();
	return m_rigidWorldMatrix;;
}

void dx3d::TransformComponent::updateWorldMatrix() noexcept
{
	if (!m_dirty) return;
	m_dirty = false;

	// Build local affine matrix: Scale * Rotation * Translation
	// Using X*Y*Z rotation order (matching toEulerAngles expectation)
	Mat4x4 localRotation = Mat4x4::rotateX(m_rotation.x) * Mat4x4::rotateY(m_rotation.y) * Mat4x4::rotateZ(m_rotation.z);

	Mat4x4 localRigid = localRotation * Mat4x4::translate(m_position);
	Mat4x4 localAffine = Mat4x4::scale(m_scale) * localRigid;

	// Check for Parent Hierarchy
	GameObject* parent = getGameObject().getParent();
	if (parent)
	{
		auto& parentTransform = parent->getTransform();

		// World = Local * ParentWorld (row-vector convention)
		m_rigidWorldMatrix = localRigid * parentTransform.getRigidWorldMatrix();
		m_affineWorldMatrix = localAffine * parentTransform.getAffineWorldMatrix();
	}
	else
	{
		m_rigidWorldMatrix = localRigid;
		m_affineWorldMatrix = localAffine;
	}
}

void dx3d::TransformComponent::markAsDirty()
{
	if (m_dirty) return;
	m_dirty = true;
	m_world.addDirtyTransformInternal(*this);

	for (auto* child : getGameObject().getChildren())
	{
		if (child)
		{
			if (auto* childTransform = child->getComponent<TransformComponent>())
			{
				childTransform->markAsDirty();
			}
		}
	}
}