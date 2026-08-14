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
    std::vector<Vec2> texcoords;
    std::vector<Vec3> normals;
    std::vector<Vertex> vertices;
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
        else if (prefix == "vt") {
            Vec2 uv;
            iss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (prefix == "vn") {
            Vec3 n;
            iss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (prefix == "f") {
            std::string vStr;
            for (int i = 0; i < 3; ++i) {
                iss >> vStr;
                if (!vStr.empty()) {
                    // Format: vertexIndex/texcoordIndex/normalIndex
                    int vIndex = 0, tIndex = 0, nIndex = 0;

                    size_t firstSlash = vStr.find('/');
                    size_t secondSlash = vStr.find('/', firstSlash + 1);

                    if (firstSlash != std::string::npos) {
                        vIndex = std::stoi(vStr.substr(0, firstSlash));
                        if (secondSlash != std::string::npos && secondSlash > firstSlash + 1) {
                            tIndex = std::stoi(vStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                            nIndex = std::stoi(vStr.substr(secondSlash + 1));
                        }
                        else {
                            // case: v/t
                            tIndex = std::stoi(vStr.substr(firstSlash + 1));
                        }
                    }
                    else {
                        // case: v only
                        vIndex = std::stoi(vStr);
                    }

                    Vertex v{};
                    v.position = positions[vIndex - 1];
                    if (tIndex > 0 && tIndex <= (int)texcoords.size())
                        v.texcoord = texcoords[tIndex - 1];
                    if (nIndex > 0 && nIndex <= (int)normals.size())
                        v.normal = normals[nIndex - 1];

                    indices.push_back(static_cast<ui32>(vertices.size()));
                    vertices.push_back(v);
                }
            }
        }
    }

    m_mesh = desc.graphicsDevice.createMesh({ vertices, indices });
}
