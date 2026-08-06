#pragma once

#include <DX3D/Core/Common.h>

#include "unordered_map"
#include "vector"
#include "string"

namespace dx3d
{
	class TextureManager final
	{

    public:
        TextureManager();

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        static TextureManager& getInstance() {
            static TextureManager instance;
            return instance;
        }

        void loadAllTextures(dx3d::ResourceManager& resManager);
        void loadTexture(dx3d::ResourceManager& resManager, std::string fileName);

        std::string getTextureNameFromFileName(std::string fileName);
        RefPtr<dx3d::TextureResource> getTexture(std::string textureName);
        std::vector<std::string> getAllTextureNames();

    private:
        inline static TextureManager* instancePtra = nullptr;

        std::unordered_map<std::string, RefPtr<dx3d::TextureResource>> m_textures{};
	};

}