#include <DX3D/Scene/Scene.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>

dx3d::GameObject* dx3d::Scene::addObject(UniquePtr<GameObject> object)
{
    if (!object) return {};

    auto ptr = object.get();
    ptr->setID(m_nextGameObjectId++);
    auto typeId = ptr->getTypeId();

    m_objects[typeId].push_back(std::move(object));
    return ptr;
}

void dx3d::Scene::addComponent(Component& component)
{
    auto typeId = component.getTypeId();
    component.setID(m_nextComponenttId++);
    m_components[typeId].push_back(&component);
}

dx3d::Component* const* dx3d::Scene::getComponentsInternal(size_t typeId, ui32* numComponents) const noexcept
{
    auto it = m_components.find(typeId);
    if (it != m_components.end())
    {
        *numComponents = static_cast<ui32>(it->second.size());
        return it->second.data();
    }

    *numComponents = 0u;
    return {};
}
