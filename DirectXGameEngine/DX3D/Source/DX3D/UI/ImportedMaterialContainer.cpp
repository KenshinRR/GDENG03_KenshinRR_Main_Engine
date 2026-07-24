#include <DX3D/Resource/ImportedMaterialContainer.h>

dx3d::ImportedMaterialContainer::ImportedMaterialContainer()
{
}

void dx3d::ImportedMaterialContainer::addMaterial(std::string key, RefPtr<MaterialResource> mat)
{
	m_materialList[key] = mat;
}

dx3d::RefPtr<dx3d::MaterialResource> dx3d::ImportedMaterialContainer::getMaterial(std::string key)
{
	return m_materialList[key];
}
