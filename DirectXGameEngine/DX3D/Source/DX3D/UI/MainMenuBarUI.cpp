#include <DX3D/UI/MainMenuBarUI.h>

#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <DX3D/EventBroadcasting/Parameters.h>

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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Add New Window", "Ctrl+N"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_WINDOW_NEW);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Editor"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_EDITOR_UNDO);
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_EDITOR_REDO);
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Edit Mode"))
			{
				Parameters params;
				params.PutExtra("IsPlayMode", false);
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_EDITOR_PLAY_MODE_CHANGED, params);
			}
			if (ImGui::MenuItem("Play Mode"))
			{
				Parameters params;
				params.PutExtra("IsPlayMode", true);
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_EDITOR_PLAY_MODE_CHANGED, params);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
