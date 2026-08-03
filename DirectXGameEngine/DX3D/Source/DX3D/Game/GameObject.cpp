#include<DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Game/World.h>

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

    // Helper lambda to calculate true accumulated World Rotation across the entire parent chain
    auto calculateWorldRotation = [](GameObject* obj) -> Vec3 {
        Vec3 totalRot = { 0.0f, 0.0f, 0.0f };
        GameObject* current = obj;
        while (current)
        {
            Vec3 rot = current->getTransform().getRotation();
            totalRot.x += rot.x;
            totalRot.y += rot.y;
            totalRot.z += rot.z;
            current = current->getParent();
        }
        return totalRot;
        };

    // 1. Get current World Position, TRUE World Rotation, and World Scale BEFORE changing parents
    Mat4x4 currentWorldMat = getTransform().getAffineWorldMatrix();

    auto row0 = currentWorldMat.row(0);
    auto row1 = currentWorldMat.row(1);
    auto row2 = currentWorldMat.row(2);
    auto row3 = currentWorldMat.row(3);

    Vec3 worldPos = { row3.x, row3.y, row3.z };
    Vec3 worldRot = calculateWorldRotation(this); // Calculated true accumulated World Rotation
    Vec3 worldScale = {
        std::sqrt(row0.x * row0.x + row0.y * row0.y + row0.z * row0.z),
        std::sqrt(row1.x * row1.x + row1.y * row1.y + row1.z * row1.z),
        std::sqrt(row2.x * row2.x + row2.y * row2.y + row2.z * row2.z)
    };

    // 2. Remove from old parent
    if (m_parent)
    {
        std::erase(m_parent->m_children, this);
    }

    // 3. Assign new parent
    m_parent = (newParent && !newParent->isDeleted()) ? newParent : nullptr;

    // 4. Calculate new local transform
    if (m_parent)
    {
        m_parent->m_children.push_back(this);

        Mat4x4 parentWorldMat = m_parent->getTransform().getAffineWorldMatrix();

        auto pRow0 = parentWorldMat.row(0);
        auto pRow1 = parentWorldMat.row(1);
        auto pRow2 = parentWorldMat.row(2);
        auto pRow3 = parentWorldMat.row(3);

        Vec3 parentWorldPos = { pRow3.x, pRow3.y, pRow3.z };
        Vec3 parentWorldRot = calculateWorldRotation(m_parent); // Parent's true World Rotation

        Vec3 parentWorldScale = {
            std::sqrt(pRow0.x * pRow0.x + pRow0.y * pRow0.y + pRow0.z * pRow0.z),
            std::sqrt(pRow1.x * pRow1.x + pRow1.y * pRow1.y + pRow1.z * pRow1.z),
            std::sqrt(pRow2.x * pRow2.x + pRow2.y * pRow2.y + pRow2.z * pRow2.z)
        };

        // Extract parent's normalized orientation axes (X, Y, Z basis vectors)
        Vec3 pAxisX = {
            (parentWorldScale.x != 0.0f) ? pRow0.x / parentWorldScale.x : 0.0f,
            (parentWorldScale.x != 0.0f) ? pRow0.y / parentWorldScale.x : 0.0f,
            (parentWorldScale.x != 0.0f) ? pRow0.z / parentWorldScale.x : 0.0f
        };
        Vec3 pAxisY = {
            (parentWorldScale.y != 0.0f) ? pRow1.x / parentWorldScale.y : 0.0f,
            (parentWorldScale.y != 0.0f) ? pRow1.y / parentWorldScale.y : 0.0f,
            (parentWorldScale.y != 0.0f) ? pRow1.z / parentWorldScale.y : 0.0f
        };
        Vec3 pAxisZ = {
            (parentWorldScale.z != 0.0f) ? pRow2.x / parentWorldScale.z : 0.0f,
            (parentWorldScale.z != 0.0f) ? pRow2.y / parentWorldScale.z : 0.0f,
            (parentWorldScale.z != 0.0f) ? pRow2.z / parentWorldScale.z : 0.0f
        };

        // Distance vector in world space
        Vec3 relPos = {
            worldPos.x - parentWorldPos.x,
            worldPos.y - parentWorldPos.y,
            worldPos.z - parentWorldPos.z
        };

        // Project relative vector onto parent's local basis axes
        Vec3 unrotatedPos = {
            relPos.x * pAxisX.x + relPos.y * pAxisX.y + relPos.z * pAxisX.z,
            relPos.x * pAxisY.x + relPos.y * pAxisY.y + relPos.z * pAxisY.z,
            relPos.x * pAxisZ.x + relPos.y * pAxisZ.y + relPos.z * pAxisZ.z
        };

        // New Local Position
        Vec3 newLocalPos = {
            (parentWorldScale.x != 0.0f) ? unrotatedPos.x / parentWorldScale.x : unrotatedPos.x,
            (parentWorldScale.y != 0.0f) ? unrotatedPos.y / parentWorldScale.y : unrotatedPos.y,
            (parentWorldScale.z != 0.0f) ? unrotatedPos.z / parentWorldScale.z : unrotatedPos.z
        };

        // New Local Scale
        Vec3 newLocalScale = {
            (parentWorldScale.x != 0.0f) ? worldScale.x / parentWorldScale.x : worldScale.x,
            (parentWorldScale.y != 0.0f) ? worldScale.y / parentWorldScale.y : worldScale.y,
            (parentWorldScale.z != 0.0f) ? worldScale.z / parentWorldScale.z : worldScale.z
        };

        // New Local Rotation (World Rotation minus Parent World Rotation)
        Vec3 newLocalRot = {
            worldRot.x - parentWorldRot.x,
            worldRot.y - parentWorldRot.y,
            worldRot.z - parentWorldRot.z
        };

        getTransform().setPosition(newLocalPos);
        getTransform().setScale(newLocalScale);
        getTransform().setRotation(newLocalRot);
    }
    else
    {
        // UNPARENTING: Restores full accumulated world rotation, position, and scale
        getTransform().setPosition(worldPos);
        getTransform().setScale(worldScale);
        getTransform().setRotation(worldRot);
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