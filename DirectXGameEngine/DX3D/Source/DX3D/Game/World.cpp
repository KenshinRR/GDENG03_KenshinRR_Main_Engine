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
#include <filesystem>
#include <ranges>

dx3d::World::World(const WorldDesc& desc) : 
    Base(desc.base), 
    m_gameContext(desc.gameContext)
{
    EventBroadcastManager::getInstance().addObserver
    (
        EventNames::ON_ADD_3D_OBJECT,
        [this]()
        {
            AddEmptyGameObject();
        }
    );
    EventBroadcastManager::getInstance().addObserver
    (
        EventNames::ON_ADD_3D_OBJECT,
        [this](dx3d::Parameters& params)
        {
            Add3DModelGameObject(params.GetStringExtra("Key", "null"));
        }
    );
}

void dx3d::World::onStart()
{
    auto numComponents = 0u;
    auto rigidBodyComponents = getComponents<RigidBodyComponent>(numComponents);
    for (auto i : std::views::iota(0u, numComponents))
    {
        RigidBodyComponent* rigidBodyComponent = rigidBodyComponents[i];
        rigidBodyComponent->onStart();
    }
}

void dx3d::World::update(f32 deltaTime)
{
    if (m_events.size())
    {
        std::swap(m_events, m_eventsSwapBuffer);

        for (auto& e : m_eventsSwapBuffer)
        {
            if (e.eventType == EventType::Create)
            {
                e.object->onCreate();  // Call onCreate on the object
            }
        }

        m_eventsSwapBuffer.clear();
    }

    for (auto&& [typeId, objects] : m_objects)
    {
        for (auto& object : objects)
        {
            object->onUpdate(deltaTime);
        }
    }

    for (auto& comp : m_dirtyTransforms)
    {
        comp->updateWorldMatrix();
    }
    m_dirtyTransforms.clear();
}

void dx3d::World::onEnd()
{
    auto numComponents = 0u;
    auto rigidBodyComponents = getComponents<RigidBodyComponent>(numComponents);
    for (auto i : std::views::iota(0u, numComponents))
    {
        RigidBodyComponent* rigidBodyComponent = rigidBodyComponents[i];
        rigidBodyComponent->onEnd();
    }
}

const std::unordered_map<size_t, std::vector<dx3d::UniquePtr<dx3d::GameObject>>>& dx3d::World::getGameObjectList()
{
    return m_objects;
}

dx3d::GameObject* dx3d::World::AddEmptyGameObject()
{
    return createGameObject<dx3d::GameObject>();
}

dx3d::GameObject* dx3d::World::Add3DModelGameObject(std::string key)
{
    // Get mesh from container
    auto imported_mesh = ImportedMeshContainer::getInstance().getMesh(key);

    if (imported_mesh == nullptr)
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
	if (!object) return {};

	auto ptr = object.get();
	auto typeId = ptr->getTypeId();

	// Store immediately in the main container
	m_objects[typeId].push_back(std::move(object));

	// Queue the onCreate event for later
    m_events.push_back({ ptr, EventType::Create });

	return ptr;
}

void dx3d::World::addComponentInternal(Component& component)
{
	auto typeId = component.getTypeId();
	m_components[typeId].push_back(&component);
}

void dx3d::World::addDirtyTransformInternal(TransformComponent& component)
{
	m_dirtyTransforms.push_back(&component);
}

dx3d::Component* const* dx3d::World::getComponentsInternal(size_t typeId, ui32* numComponents) const noexcept
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