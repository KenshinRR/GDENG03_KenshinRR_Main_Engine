#include "ImportedMeshContainer.h"

dx3d::ImportedMeshContainer::ImportedMeshContainer()
{
}

void dx3d::ImportedMeshContainer::addMesh(std::string key, RefPtr<MeshResource> mesh)
{
	m_meshList[key] = mesh;
}

dx3d::RefPtr<dx3d::MeshResource> dx3d::ImportedMeshContainer::getMesh(std::string key)
{
	return m_meshList[key];
}
