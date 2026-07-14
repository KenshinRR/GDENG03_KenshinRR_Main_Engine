#include <DX3D/UI/SceneUI.h>

#include <DX3D/Game/World.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CubeComponent.h>

#include <DX3D/Component/CameraComponent.h>
#include <DX3D/Graphics/GraphicsDevice.h>

// ImGui's DragFloat3 range uses FLT_MAX.
#include <cfloat>
#include <imgui.h>

dx3d::SceneUI::SceneUI(const BaseDesc& desc) : BaseUI(desc)
{
}

void dx3d::SceneUI::draw()
{
	if (ImGui::Begin("Credits"))
	{
		if (ImGui::BeginTabBar("##CREDITS")) // create tab bar with id
		{
			if (ImGui::BeginTabItem("About"))
			{
				drawAboutTabItem();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

dx3d::SceneUI::~SceneUI()
{
}

void dx3d::SceneUI::drawAboutTabItem()
{
	ImGui::Text("Direct X KenshinRR Engine");
	ImGui::Text("Programmed by Kenshin R. Reblando");
	ImGui::Text("Based on Pardcode's C++ 3D Game Engine");
}
