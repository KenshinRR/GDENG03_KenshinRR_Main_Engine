#include <DX3D/UI/SceneStateUI.h>

#include <imgui.h>

#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>

dx3d::SceneStateUI::SceneStateUI(const BaseDesc& desc) : BaseUI(desc)
{
}

void dx3d::SceneStateUI::draw()
{
	if (m_showUI)
	{
		if (ImGui::Begin("Scene State", &m_showUI))
		{
			// Add GameObject button
			if (ImGui::Button(m_currentStateLabel.c_str()))
			{
				if (m_currentStateLabel == "Play")
				{
					m_currentStateLabel = "Stop";
					EventBroadcastManager::getInstance().postEvent(EventNames::ON_SCENE_PLAY);
				}
				else
				{
					m_currentStateLabel = "Play";
					EventBroadcastManager::getInstance().postEvent(EventNames::ON_SCENE_STOP);
				}

			}
			// Add GameObject button
			if (ImGui::Button("Pause"))
			{
				EventBroadcastManager::getInstance().postEvent(EventNames::ON_SCENE_PAUSE);
			}
			// Add GameObject button
			if (ImGui::Button("Frame Step"))
			{

			}
		}
		ImGui::End();
	}
}

dx3d::SceneStateUI::~SceneStateUI()
{
}
