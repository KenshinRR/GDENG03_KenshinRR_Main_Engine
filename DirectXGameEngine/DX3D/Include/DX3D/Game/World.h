#pragma once
#include <DX3D/Core/Common.h>
#include <DX3D/Core/Base.h>
#include <DX3D/Core/Identifiable.h>
#include <DX3D/Scene/Scene.h>
#include <DX3D/Scene/SceneManager.h>
#include <unordered_map>
#include <vector>

namespace dx3d
{
    class World final : public Base
    {
    public:
        explicit World(const WorldDesc& desc);

        template <typename T>
        T* createGameObject() requires IsRegistered<GameObject, T>
        {
            static_assert(std::is_base_of<GameObject, T>::value, "T must inherit from dx3d::GameObject.");
            static_assert(HasTypeId<T>, "T needs a unique TypeId.");
            UniquePtr<GameObject> e = std::make_unique<T>(GameObjectDesc{
                {m_logger},
                m_gameContext,
                *this
                });
            return static_cast<T*>(createGameObjectInternal(e));
        }

        template <typename T> requires IsRegistered<Component, T>
        T* const* getComponents(ui32& numComponents) const noexcept
        {
            return reinterpret_cast<T* const*>(SceneManager::getInstance().getActiveScene()->getComponentsInternal(T::GetTypeId(), &numComponents));
        }

        void onStart();
        void update(f32 deltaTime);
        void onEnd();

        const std::unordered_map<size_t, std::vector<UniquePtr<GameObject>>>& getGameObjectList();

        GameObject* AddEmptyGameObject();
        GameObject* Add3DModelGameObject(std::string key);

    private:
        GameObject* createGameObjectInternal(UniquePtr<GameObject>& object);

        void addDirtyTransformInternal(TransformComponent& component);

        enum class EventType { Create = 0 };
        struct GameObjectEvent
        {
            GameObject* object{};
            EventType eventType{};
        };

    private:
        GameContext m_gameContext;

        std::vector<TransformComponent*> m_dirtyTransforms{};
        std::vector<GameObjectEvent> m_events{};
        std::vector<GameObjectEvent> m_eventsSwapBuffer{};

        friend class GameObject;
        friend class TransformComponent;
    };
}
