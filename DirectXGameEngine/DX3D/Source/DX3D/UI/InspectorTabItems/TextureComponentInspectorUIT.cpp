#include <DX3D/UI/InspectorTabItems/TextureComponentInspectorUIT.h>

#include <DX3D/Resource/TextureManager.h>
#include <DX3D/Resource/MaterialResource.h>
#include <DX3D/Resource/TextureResource.h>

#include <DX3D/Graphics/Texture.h>

#include <imgui.h>

#include <cstring>

dx3d::TextureComponentInspectorUIT::TextureComponentInspectorUIT()
{

}

void dx3d::TextureComponentInspectorUIT::draw(MaterialResource* matResource)
{
	if (matResource == nullptr)
	{
		ImGui::Text("No texture found.");
		return;
	}

	std::vector<std::string> m_textureStrings;

	m_textureStrings = TextureManager::getInstance().getAllTextureNames();

	m_textureNames.clear();
	m_textureNames.reserve(m_textureStrings.size());
	for (auto& s : m_textureStrings) {
		m_textureNames.push_back(s.c_str());
	}

	std::string textureName = TextureManager::getInstance().getStringKey(matResource->getTexture(0));
	static int selectedItem = getTextureNameKey(textureName);
	if (ImGui::Combo("Dropdown", &selectedItem, m_textureNames.data(), (int)m_textureNames.size())) {
		//matResource->setTexture(0, TextureManager::getInstance().getTexture(textureName));
	}
}

int dx3d::TextureComponentInspectorUIT::getTextureNameKey(std::string textName)
{
	const char* cTarget = textName.c_str();

	for (size_t i = 0; i < m_textureNames.size(); ++i) {
		if (std::strcmp(m_textureNames[i], cTarget) == 0) {
			return static_cast<int>(i);
		}
	}
	return -1; // not found
}
