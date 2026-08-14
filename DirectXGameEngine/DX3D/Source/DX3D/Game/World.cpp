#include <DX3D/Game/World.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/MeshComponent.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/RigidBodyComponent.h>
#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <DX3D/EventBroadcasting/Parameters.h>
#include <DX3D/Graphics/Mesh/ImportedMeshContainer.h>
#include <DX3D/Resource/ImportedMaterialContainer.h>
#include <DX3D/Scene/SceneManager.h>
#include <DX3D/Scene/Scene.h>
#include <ranges>

dx3d::World::World(const WorldDesc& desc) :
    Base(desc.base),
    m_gameContext(desc.gameContext)
{
    EventBroadcastManager::getInstance().addObserver
    (
        EventNames::ON_ADD_3D_OBJECT,
        [this]() { AddEmptyGameObject(); }
    );
    EventBroadcastManager::getInstance().addObserver
    (
        EventNames::ON_ADD_3D_OBJECT,
        [this](dx3d::Parameters& params) { Add3DModelGameObject(params.GetStringExtra("Key", "null")); }
    );
}

void dx3d::World::onStart()
{
    auto numComponents = 0u;
    auto rigidBodyComponents = getComponents<RigidBodyComponent>(numComponents);
    for (auto i : std::views::iota(0u, numComponents))
        rigidBodyComponents[i]->onStart();
}

void dx3d::World::update(f32 deltaTime)
{
    if (m_events.size())
    {
        std::swap(m_events, m_eventsSwapBuffer);

        for (auto& e : m_eventsSwapBuffer)
        {
            if (e.eventType == EventType::Create)
                e.object->onCreate();
        }

        m_eventsSwapBuffer.clear();
    }

    Scene* activeScene = SceneManager::getInstance().getActiveScene();
    if (!activeScene) return;

    // Update objects
    for (auto&& [typeId, objects] : activeScene->getObjects())
    {
        for (auto& object : objects)
            object->onUpdate(deltaTime);
    }

    // Update dirty transforms
    for (auto& comp : m_dirtyTransforms)
        comp->updateWorldMatrix();
    m_dirtyTransforms.clear();
}

void dx3d::World::onEnd()
{
    auto numComponents = 0u;
    auto rigidBodyComponents = getComponents<RigidBodyComponent>(numComponents);
    for (auto i : std::views::iota(0u, numComponents))
        rigidBodyComponents[i]->onEnd();
}

const std::unordered_map<size_t, std::vector<dx3d::UniquePtr<dx3d::GameObject>>>& dx3d::World::getGameObjectList()
{
    return m_scene.getObjects();
}

dx3d::GameObject* dx3d::World::AddEmptyGameObject()
{
    return createGameObject<dx3d::GameObject>();
}

dx3d::GameObject* dx3d::World::Add3DModelGameObject(std::string key)
{
    auto imported_mesh = ImportedMeshContainer::getInstance().getMesh(key);
    if (!imported_mesh)
    {
        DX3DLogError("Mesh not imported {}", key);
        return nullptr;
    }

    GameObject* new_obj = createGameObject<dx3d::GameObject>();
    new_obj->setName(key);
    auto new_obj_comp = new_obj->createOrGetComponent<dx3d::MeshComponent>();
    new_obj_comp->setMaterial(ImportedMaterialContainer::getInstance().getMaterial("Basic"));
    new_obj_comp->setMesh(imported_mesh->getMesh());

    return new_obj;
}

dx3d::GameObject* dx3d::World::createGameObjectInternal(UniquePtr<GameObject>& object)
{
    auto ptr = m_scene.addObject(std::move(object));
    if (ptr)
        m_events.push_back({ ptr, EventType::Create });
    return ptr;
}

void dx3d::World::addDirtyTransformInternal(TransformComponent& component)
{
    m_dirtyTransforms.push_back(&component);
}
