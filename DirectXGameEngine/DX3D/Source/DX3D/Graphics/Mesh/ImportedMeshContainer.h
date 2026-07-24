#pragma once
#include <DX3D/Core/Common.h>
#include <unordered_map>
#include <DX3D/Resource/MeshResource.h>

namespace dx3d
{
	class ImportedMeshContainer
	{
    public:
        ImportedMeshContainer();

        ImportedMeshContainer(const ImportedMeshContainer&) = delete;
        ImportedMeshContainer& operator=(const ImportedMeshContainer&) = delete;

        static ImportedMeshContainer& getInstance() {
            static ImportedMeshContainer instance;
            return instance;
        }

        void addMesh(std::string key, RefPtr<MeshResource> mesh);
        RefPtr<MeshResource> getMesh(std::string key);

    private:
        inline static ImportedMeshContainer* instancePtra = nullptr;
        std::unordered_map<std::string, RefPtr<MeshResource>> m_meshList{};
	};
}