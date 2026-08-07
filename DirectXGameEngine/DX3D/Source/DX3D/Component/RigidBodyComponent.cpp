#include <DX3D/Component/RigidBodyComponent.h>
#include <DX3D/Physics/PhysicsManager.h>

dx3d::RigidBodyComponent::RigidBodyComponent(const ComponentDesc& data) : Component(data)
{
}

void dx3d::RigidBodyComponent::setTransformComponent(TransformComponent* tc)
{
	m_transformComponent = tc;
}

void dx3d::RigidBodyComponent::onStart()
{
	m_initial_transform["Position"] = m_transformComponent->getPosition();
	m_initial_transform["Scale"] = m_transformComponent->getScale();
	m_initial_transform["Rotation"] = m_transformComponent->getRotation();
}

void dx3d::RigidBodyComponent::onEnd()
{
	m_transformComponent->setPosition(m_initial_transform["Position"]);
	m_transformComponent->setScale(m_initial_transform["Scale"]);
	m_transformComponent->setRotation(m_initial_transform["Rotation"]);

	PhysicsManager::getInstance().syncComponentToPhysics(m_transformComponent);
}
