#include <DX3D/UI/SceneUI.h>

#include <DX3D/Game/World.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CubeComponent.h>

#include <DX3D/Component/CameraComponent.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/Texture.h>
#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>

#include <DX3D/Resource/TextureResource.h>

// ImGui's DragFloat3 range uses FLT_MAX.
#include <cfloat>
#include <imgui.h>

dx3d::SceneUI::SceneUI(const BaseDesc& desc) : BaseUI(desc)
{
	float color[4] = { 0.2f, 0.5f, 0.8f, 1.0f };
	m_color = color;
	EventBroadcastManager::getInstance().addObserver(
		EventNames::ON_CREDITS_POPUP,
		[this]() { m_showCredits = true; }
	);
	EventBroadcastManager::getInstance().addObserver(
		EventNames::ON_COLORPICKER_POPUP,
		[this]() { m_showColorPicker = true; }
	);
}

void dx3d::SceneUI::draw()
{
	if (m_showCredits)
	{
		if (ImGui::Begin("Credits", &m_showCredits))
		{
			if (ImGui::BeginTabBar("##CREDITS")) // create tab bar with id
			{
				if (ImGui::BeginTabItem("About"))
				{
					ImGui::Image(m_logo, ImVec2(128, 128)); // show PNG icon
					drawAboutTabItem();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}


	if (m_showColorPicker)
	{
		if (ImGui::Begin("Color Picker Screen", &m_showColorPicker))
		{
			if (ImGui::ColorPicker3("Pick a Color", m_color))
			{
				// Color changed, use updated 'color' array
			}
		}
		ImGui::End();
	}
}

dx3d::SceneUI::~SceneUI()
{
}

void dx3d::SceneUI::setLogo(RefPtr<TextureResource> logoTexture)
{
	Texture& txtLogo = (logoTexture)->getTexture();
	m_logo = (ImTextureID)(txtLogo.getSRV());
}

void dx3d::SceneUI::drawAboutTabItem()
{
	ImGui::Text("Direct X KenshinRR Engine");
	ImGui::Text("Programmed by Kenshin R. Reblando");
	ImGui::Text("\nAcknowledgements");
	ImGui::Text("Pardcode's C++ 3D Game Engine");
	ImGui::Text("Doc Neil's GDENG courses");
}
