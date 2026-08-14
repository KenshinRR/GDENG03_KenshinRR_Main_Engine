#include <DX3D/Scene/SceneManager.h>

namespace dx3d
{
    SceneManager& SceneManager::getInstance()
    {
        static SceneManager instance;
        return instance;
    }

    Scene* SceneManager::createScene(size_t sceneId)
    {
        auto scene = std::make_unique<Scene>();
        Scene* ptr = scene.get();
        m_scenes[sceneId] = std::move(scene);
        return ptr;
    }

    void SceneManager::setActiveScene(size_t sceneId)
    {
        auto it = m_scenes.find(sceneId);
        if (it != m_scenes.end())
            m_activeScene = it->second.get();
    }

    Scene* SceneManager::getActiveScene() const
    {
        return m_activeScene;
    }

    Scene* SceneManager::getScene(size_t sceneId) const
    {
        auto it = m_scenes.find(sceneId);
        return (it != m_scenes.end()) ? it->second.get() : nullptr;
    }
}
