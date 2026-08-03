#include<DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Game/World.h>
#include <cmath>
#include <algorithm>

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

    // Capture current World Matrix BEFORE hierarchy change
    Mat4x4 worldMat = getTransform().getAffineWorldMatrix();

    // Unlink from current parent
    if (m_parent)
    {
        std::erase(m_parent->m_children, this);
    }

    // Assign new parent
    m_parent = (newParent && !newParent->isDeleted()) ? newParent : nullptr;

    if (m_parent)
    {
        m_parent->m_children.push_back(this);

        // Get parent's world matrix
        Mat4x4 parentWorldMat = m_parent->getTransform().getAffineWorldMatrix();
        Mat4x4 invParentMat = Mat4x4::inverse(parentWorldMat);

        // Calculate local matrix: Local = World * Inverse(ParentWorld)
        Mat4x4 localMat = worldMat * invParentMat;

        // Extract position from row 3
        Vec4 row3 = localMat.row(3);
        Vec3 newPos = { row3.x, row3.y, row3.z };

        // Extract ROW vectors (not columns!) for toEulerAngles
        Vec4 row0 = localMat.row(0);
        Vec4 row1 = localMat.row(1);
        Vec4 row2 = localMat.row(2);

        Vec3 r0 = { row0.x, row0.y, row0.z };
        Vec3 r1 = { row1.x, row1.y, row1.z };
        Vec3 r2 = { row2.x, row2.y, row2.z };

        // Extract scale from row lengths
        Vec3 newScale;
        newScale.x = r0.length();
        newScale.y = r1.length();
        newScale.z = r2.length();

        // Remove scale from rotation vectors
        if (newScale.x > 0.00001f) r0 = r0 / newScale.x;
        if (newScale.y > 0.00001f) r1 = r1 / newScale.y;
        if (newScale.z > 0.00001f) r2 = r2 / newScale.z;

        // toEulerAngles expects ROW vectors
        Vec3 newRot = Mat4x4::toEulerAngles(r0, r1, r2);

        // Apply the calculated local transform
        getTransform().setPosition(newPos);
        getTransform().setRotation(newRot);
        getTransform().setScale(newScale);
    }
    else
    {
        // Unparenting: extract world matrix components
        Vec4 worldRow3 = worldMat.row(3);
        Vec3 worldPos = { worldRow3.x, worldRow3.y, worldRow3.z };

        Vec4 worldRow0 = worldMat.row(0);
        Vec4 worldRow1 = worldMat.row(1);
        Vec4 worldRow2 = worldMat.row(2);

        Vec3 wr0 = { worldRow0.x, worldRow0.y, worldRow0.z };
        Vec3 wr1 = { worldRow1.x, worldRow1.y, worldRow1.z };
        Vec3 wr2 = { worldRow2.x, worldRow2.y, worldRow2.z };

        Vec3 worldScale;
        worldScale.x = wr0.length();
        worldScale.y = wr1.length();
        worldScale.z = wr2.length();

        if (worldScale.x > 0.00001f) wr0 = wr0 / worldScale.x;
        if (worldScale.y > 0.00001f) wr1 = wr1 / worldScale.y;
        if (worldScale.z > 0.00001f) wr2 = wr2 / worldScale.z;

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