#pragma once
#include <DX3D/Scene/Scene.h>
#include <unordered_map>
#include <memory>

namespace dx3d
{
    class SceneManager
    {
    public:
        SceneManager();

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        static SceneManager& getInstance() {
            static SceneManager instance;
            return instance;
        }

        Scene* createScene(size_t sceneId);
        void setActiveScene(size_t sceneId);

        Scene* getActiveScene() const;
        Scene* getScene(size_t sceneId) const;

        void saveSceneToFile(size_t sceneId, const std::string& filename);

    private:
        std::unordered_map<size_t, std::unique_ptr<Scene>> m_scenes{};
        Scene* m_activeScene{ nullptr };
        inline static SceneManager* instancePtra = nullptr;
    };
}
