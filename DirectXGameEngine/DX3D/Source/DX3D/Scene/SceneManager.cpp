#include <DX3D/Scene/SceneManager.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CameraComponent.h>
#include <DX3D/Component/RigidBodyComponent.h>
#include <DX3D/Component/MeshComponent.h>
#include <DX3D/Math/Rect.h>

#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>

#include <fstream>

#include <json.hpp>

#include <string>

using json = nlohmann::json;

dx3d::SceneManager::SceneManager()
{
    EventBroadcastManager::getInstance().addObserver
    (
        EventNames::ON_SCENE_SAVE,
        [this]() { 
            getInstance().saveSceneToFile(
                getInstance().getActiveScene()->getID(),
                "Scene " + std::to_string(getInstance().getActiveScene()->getID())
                ); 
        }
    );
}

dx3d::Scene* dx3d::SceneManager::createScene(size_t sceneId)
{
    auto scene = std::make_unique<Scene>();
    Scene* ptr = scene.get();
    scene->setID(sceneId);
    m_scenes[sceneId] = std::move(scene);
    return ptr;
}

void dx3d::SceneManager::setActiveScene(size_t sceneId)
{
    auto it = m_scenes.find(sceneId);
    if (it != m_scenes.end())
        m_activeScene = it->second.get();
}

dx3d::Scene* dx3d::SceneManager::getActiveScene() const
{
    return m_activeScene;
}

dx3d::Scene* dx3d::SceneManager::getScene(size_t sceneId) const
{
    auto it = m_scenes.find(sceneId);
    return (it != m_scenes.end()) ? it->second.get() : nullptr;
}

void dx3d::SceneManager::saveSceneToFile(size_t sceneId, const std::string& filename)
{
    Scene* scene = getScene(sceneId);
    if (!scene) return;

    json sceneJson = json::array();

    for (auto&& [typeId, objects] : scene->getObjects())
    {
        for (auto& objPtr : objects)
        {
            GameObject* obj = objPtr.get();
            if (!obj) continue;

            json goJson;
            goJson["GameObject"]["m_Name"] = obj->getName();

            // Components list
            json compList = json::array();
            for (const auto& [typeId, component] : obj->getComponentList()) {
                if (component) {
                    compList.push_back({ {"component", component->getID()} });
                }
            }
            goJson["GameObject"]["m_Component"] = compList;
            goJson["GameObject"]["m_ID"] = obj->getID();

            // Transform
            if (auto* transform = obj->getComponent<TransformComponent>())
            {
                goJson["Transform"] = {
                    {"m_GameObject", obj->getID()},
                    {"m_LocalPosition", {{"x", transform->getPosition().x}, {"y", transform->getPosition().y}, {"z", transform->getPosition().z}}},
                    {"m_LocalRotation", {{"x", transform->getRotation().x}, {"y", transform->getRotation().y}, {"z", transform->getRotation().z}}},
                    {"m_LocalScale",    {{"x", transform->getScale().x}, {"y", transform->getScale().y}, {"z", transform->getScale().z}}}
                };
            }

            // Camera
            if (auto* camera = obj->getComponent<CameraComponent>())
            {
                goJson["Camera"] = {
                    {"m_GameObject", obj->getID()},
                    {"m_nearPlane", camera->getNearPlane()},
                    {"m_farPlane", camera->getFarPlane()},
                    {"m_fieldOfView", camera->getFieldOfView()},
                    {"m_viewportSize", {{"w", camera->getViewportSize().width}, {"h", camera->getViewportSize().height}}}
                };
            }

            // Rigidbody
            if (auto* rb = obj->getComponent<RigidBodyComponent>())
            {
                goJson["Rigidbody"] = {
                    {"m_GameObject", obj->getID()},
                };
            }

            // Mesh
            //if (auto* meshComp = obj->getComponent<MeshComponent>())
            //{
            //    goJson["Mesh"] = {
            //        {"m_MeshID", meshComp->getMesh()->getID()},
            //        {"m_MaterialID", meshComp->getMaterial()->getID()}
            //        // Add textures if needed
            //    };
            //}

            sceneJson.push_back(goJson);
        }
    }

    // Construct the full path
    std::filesystem::path base = std::filesystem::current_path().parent_path();
    std::filesystem::path sceneDir = base / "DirectXGameEngine/Game/Assets/Scenes";
    std::filesystem::create_directories(sceneDir); // ensure folder exists

    std::filesystem::path fullPath = sceneDir / filename;

    // Write to file
    std::ofstream fout(fullPath);
    fout << sceneJson.dump(4);

}
