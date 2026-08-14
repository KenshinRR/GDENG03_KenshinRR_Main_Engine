#pragma once
#include <DX3D/Core/Common.h>

#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>

#include <unordered_map>
#include <vector>
#include <memory>

namespace dx3d
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        // Add a new GameObject (ownership transferred)
        GameObject* addObject(UniquePtr<GameObject> object);

        // Register a component (non-owning)
        void addComponent(Component& component);

        // Accessors
        const std::unordered_map<size_t, std::vector<UniquePtr<GameObject>>>& getObjects() const { return m_objects; }
        const std::unordered_map<size_t, std::vector<Component*>>& getComponents() const { return m_components; }

        // Internal lookup
        Component* const* getComponentsInternal(size_t typeId, ui32* numComponents) const noexcept;

        void setID(size_t id) { m_ID = id; }
        size_t getID() { return m_ID; }

    private:
        std::unordered_map<size_t, std::vector<UniquePtr<GameObject>>> m_objects{};
        std::unordered_map<size_t, std::vector<Component*>> m_components{};
        size_t m_ID{};

        size_t m_nextGameObjectId{ 0 };
        size_t m_nextComponenttId{ 0 };
    };
}
