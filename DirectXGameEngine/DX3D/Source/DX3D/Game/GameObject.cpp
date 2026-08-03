#include<DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Game/World.h>
#include <cmath>
#include <algorithm>
#include <Windows.h>

dx3d::GameObject::GameObject(const GameObjectDesc& desc) : 
	Identifiable(desc.base),
	m_world(desc.world), 
	m_gameContext(desc.gameContext),
	m_windowInput(desc.windowInput)
{
	m_transform = createOrGetComponent<TransformComponent>();
}

dx3d::TransformComponent& dx3d::GameObject::getTransform() noexcept
{
	return *m_transform;
}

dx3d::Component* dx3d::GameObject::createComponentInternal(UniquePtr<Component>& component)
{
	if (!component) return {};
	auto typeId = component->getTypeId();
	auto ptr = component.get();
	if (m_components.find(typeId) != m_components.end()) return {};
	m_components.emplace(typeId, std::move(component));
	m_world.addComponentInternal(*ptr);
	return ptr;
}

dx3d::World& dx3d::GameObject::getWorld() noexcept
{
	return m_world;
}

dx3d::InputSystem& dx3d::GameObject::getInputSystem() noexcept
{
	// Prefer window‑specific input if available
	if (m_windowInput) return *m_windowInput;
	return m_gameContext.input;
}

dx3d::ResourceManager& dx3d::GameObject::getResourceManager() noexcept
{
	return m_gameContext.resourceManager;
}

void dx3d::GameObject::setName(std::string name)
{
	m_name = name;
}

void dx3d::GameObject::setParent(GameObject* newParent)
{
    if (m_parent == newParent || newParent == this) return;
    if (newParent && !newParent->isDeleted() && newParent->isDescendantOf(this)) return;

    Mat4x4 worldMat = getTransform().getAffineWorldMatrix();

    if (m_parent)
    {
        std::erase(m_parent->m_children, this);
    }

    m_parent = (newParent && !newParent->isDeleted()) ? newParent : nullptr;

    if (m_parent)
    {
        m_parent->m_children.push_back(this);

        Mat4x4 parentWorldMat = m_parent->getTransform().getAffineWorldMatrix();
        Mat4x4 invParentMat = Mat4x4::inverse(parentWorldMat);
        Mat4x4 localMat = worldMat * invParentMat;

        // Extract position from row 3
        Vec3 newPos = { localMat.row(3).x, localMat.row(3).y, localMat.row(3).z };

        // Extract columns of 3x3 part (scale * rotation)
        Vec3 c0 = { localMat.row(0).x, localMat.row(1).x, localMat.row(2).x };
        Vec3 c1 = { localMat.row(0).y, localMat.row(1).y, localMat.row(2).y };
        Vec3 c2 = { localMat.row(0).z, localMat.row(1).z, localMat.row(2).z };

        // Extract scale from column lengths
        Vec3 newScale = { c0.length(), c1.length(), c2.length() };

        // Remove scale to get rotation columns
        if (newScale.x > 0.00001f) c0 = c0 / newScale.x;
        if (newScale.y > 0.00001f) c1 = c1 / newScale.y;
        if (newScale.z > 0.00001f) c2 = c2 / newScale.z;

        // Orthogonalize rotation (handles shear from non-uniform parent scale)
        Vec3::normalize(c0);
        c1 = c1 - c0 * Vec3::dot(c0, c1);
        Vec3::normalize(c1);
        c2 = Vec3::cross(c0, c1);

        // Convert rotation columns to rows for toEulerAngles
        Vec3 rotRow0 = { c0.x, c1.x, c2.x };
        Vec3 rotRow1 = { c0.y, c1.y, c2.y };
        Vec3 rotRow2 = { c0.z, c1.z, c2.z };

        Vec3 newRot = Mat4x4::toEulerAngles(rotRow0, rotRow1, rotRow2);

        getTransform().setPosition(newPos);
        getTransform().setRotation(newRot);
        getTransform().setScale(newScale);
    }
    else
    {
        Vec3 worldPos = { worldMat.row(3).x, worldMat.row(3).y, worldMat.row(3).z };

        Vec3 wc0 = { worldMat.row(0).x, worldMat.row(1).x, worldMat.row(2).x };
        Vec3 wc1 = { worldMat.row(0).y, worldMat.row(1).y, worldMat.row(2).y };
        Vec3 wc2 = { worldMat.row(0).z, worldMat.row(1).z, worldMat.row(2).z };

        Vec3 worldScale = { wc0.length(), wc1.length(), wc2.length() };

        if (worldScale.x > 0.00001f) wc0 = wc0 / worldScale.x;
        if (worldScale.y > 0.00001f) wc1 = wc1 / worldScale.y;
        if (worldScale.z > 0.00001f) wc2 = wc2 / worldScale.z;

        Vec3::normalize(wc0);
        wc1 = wc1 - wc0 * Vec3::dot(wc0, wc1);
        Vec3::normalize(wc1);
        wc2 = Vec3::cross(wc0, wc1);

        Vec3 wr0 = { wc0.x, wc1.x, wc2.x };
        Vec3 wr1 = { wc0.y, wc1.y, wc2.y };
        Vec3 wr2 = { wc0.z, wc1.z, wc2.z };

        Vec3 worldRot = Mat4x4::toEulerAngles(wr0, wr1, wr2);

        getTransform().setPosition(worldPos);
        getTransform().setRotation(worldRot);
        getTransform().setScale(worldScale);
    }
}

bool dx3d::GameObject::isDescendantOf(const GameObject * potentialAncestor) const
{
	if (!potentialAncestor) return false;
	const GameObject* current = m_parent;
	while (current)
	{
		if (current == potentialAncestor) return true;
		current = current->m_parent;
	}
	return false;
}

bool dx3d::GameObject::isActiveInHierarchy() const noexcept
{
	//If the object itself is disabled or deleted, return false
	if (!m_enabled || m_deleted) return false;

	//If the object has a parent, recursively check if the parent is active in hierarchy
	if (m_parent) return m_parent->isActiveInHierarchy();

	return true;
}

dx3d::Component* dx3d::GameObject::getComponentInternal(size_t id)
{
	auto it = m_components.find(id);
	if (it == m_components.end()) return {};
	return it->second.get();
}