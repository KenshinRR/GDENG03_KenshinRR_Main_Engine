#pragma once
#include <DX3D/Core/Common.h>
#include <unordered_map>
#include <DX3D/Resource/MaterialResource.h>

namespace dx3d
{
    class ImportedMaterialContainer
    {
    public:
        ImportedMaterialContainer();

        ImportedMaterialContainer(const ImportedMaterialContainer&) = delete;
        ImportedMaterialContainer& operator=(const ImportedMaterialContainer&) = delete;

        static ImportedMaterialContainer& getInstance() {
            static ImportedMaterialContainer instance;
            return instance;
        }

        void addMaterial(std::string key, RefPtr<MaterialResource> mat);
        RefPtr<MaterialResource> getMaterial(std::string key);

    private:
        inline static ImportedMaterialContainer* instancePtra = nullptr;
        std::unordered_map<std::string, RefPtr<MaterialResource>> m_materialList{};
    };
}