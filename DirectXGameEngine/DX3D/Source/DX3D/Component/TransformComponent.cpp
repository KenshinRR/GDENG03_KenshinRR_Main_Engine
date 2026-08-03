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

    // Compute local matrices (same as before)
    Mat4x4 localRigid =
        Mat4x4::rotateX(m_rotation.x) *
        Mat4x4::rotateY(m_rotation.y) *
        Mat4x4::rotateZ(m_rotation.z) *
        Mat4x4::translate(m_position);

    Mat4x4 localAffine =
        Mat4x4::scale(m_scale) *
        localRigid;

    // Check for Parent Hierarchy
    GameObject* parent = getGameObject().getParent();
    if (parent)
    {
        auto& parentTransform = parent->getTransform();

        // Force parent to update first
        parentTransform.updateWorldMatrix();

        // Get parent's WORLD values directly
        Vec3 parentWorldPos = parentTransform.m_worldPosition;
        Vec3 parentWorldRot = parentTransform.m_worldRotation;
        Vec3 parentWorldScale = parentTransform.m_worldScale;

        // Compute our world values from parent's world + our local
        m_worldPosition = parentWorldPos + m_position;
        m_worldRotation = parentWorldRot + m_rotation;
        m_worldScale.x = parentWorldScale.x * m_scale.x;
        m_worldScale.y = parentWorldScale.y * m_scale.y;
        m_worldScale.z = parentWorldScale.z * m_scale.z;

        // Still compute matrices for rendering
        Mat4x4 worldScaleMat = Mat4x4::scale(m_worldScale);
        Mat4x4 worldRotMat =
            Mat4x4::rotateX(m_worldRotation.x) *
            Mat4x4::rotateY(m_worldRotation.y) *
            Mat4x4::rotateZ(m_worldRotation.z);
        Mat4x4 worldTransMat = Mat4x4::translate(m_worldPosition);

        m_rigidWorldMatrix = worldRotMat * worldTransMat;
        m_affineWorldMatrix = worldScaleMat * m_rigidWorldMatrix;
    }
    else
    {
        // No parent - world = local
        m_worldPosition = m_position;
        m_worldRotation = m_rotation;
        m_worldScale = m_scale;

        m_rigidWorldMatrix = localRigid;
        m_affineWorldMatrix = localAffine;
    }
}

dx3d::Vec3 dx3d::TransformComponent::getWorldPosition()
{
    updateWorldMatrix();
    return m_worldPosition;
}

dx3d::Vec3 dx3d::TransformComponent::getWorldRotation()
{
    updateWorldMatrix();
    return m_worldRotation;
}

dx3d::Vec3 dx3d::TransformComponent::getWorldScale()
{
    updateWorldMatrix();
    return m_worldScale;
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