#include <DX3D/UI/MainMenuBarUI.h>

#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>

// ImGui's DragFloat3 range uses FLT_MAX.
#include <cfloat>
#include <imgui.h>

dx3d::MainMenuBarUI::MainMenuBarUI(const BaseDesc& desc) : BaseUI(desc)
{
}

dx3d::MainMenuBarUI::~MainMenuBarUI()
{
}

void dx3d::MainMenuBarUI::draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Toggle Color Picker"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_COLORPICKER_POPUP);
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_SCENE_SAVE);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Credits"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_CREDITS_POPUP);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Activity"))
		{
			if (ImGui::MenuItem("Spawn 20 Cubes"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ACTIVITY_SPAWN20CUBES);
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}
