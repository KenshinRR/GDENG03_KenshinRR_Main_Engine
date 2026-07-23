#include <DX3D/Resource/MeshResource.h>
#include <DX3D/Graphics/Mesh/Mesh.h>
#include <DX3D/Core/Common.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <filesystem>
#include <fstream>
#include <sstream>

dx3d::MeshResource::MeshResource(const MeshResourceDesc& desc)
    : Resource(desc.base), m_graphicsDevice(desc.graphicsDevice)
{
    std::filesystem::path meshFile = desc.base.path;
    auto meshFileStr = meshFile.string();

    if (!std::filesystem::exists(meshFile)) {
        DX3DLogThrowError("Mesh file {} does not exist.", meshFileStr.c_str());
    }

    std::ifstream file(meshFile.string());
    if (!file.is_open()) {
        DX3DLogThrowError("Failed to open mesh file {}", meshFileStr.c_str());
    }

    std::vector<Vec3> positions;
    std::vector<ui32> indices;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "f") {
            std::string vStr;
            for (int i = 0; i < 3; ++i) {
                iss >> vStr;
                if (!vStr.empty()) {
                    // Split "vertex/texcoord/normal"
                    size_t firstSlash = vStr.find('/');
                    int vIndex = std::stoi(vStr.substr(0, firstSlash));
                    indices.push_back(static_cast<ui32>(vIndex - 1));
                }
            }
        }

    }

    std::vector<Vertex> vertices;
    for (auto& pos : positions) {
        Vertex v{};
        v.position = pos;
        vertices.push_back(v);
    }

    m_mesh = desc.graphicsDevice.createMesh({ vertices, indices });
}
