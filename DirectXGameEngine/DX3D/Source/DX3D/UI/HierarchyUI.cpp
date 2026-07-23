#include <DX3D/UI/HierarchyUI.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <DX3D/EventBroadcasting/Parameters.h>

dx3d::HierarchyUI::HierarchyUI(const BaseDesc& desc) : BaseUI(desc)
{
}

dx3d::HierarchyUI::~HierarchyUI()
{
}

void dx3d::HierarchyUI::draw()
{
	if (m_showHierarchy)
	{
		if (ImGui::Begin("Hierarchy", &m_showHierarchy))
		{
            // Make sure we actually have a list set
            if (!m_gameObjects || m_gameObjects->empty())
            {
                ImGui::Text("No game objects.");
                ImGui::End();
                return;
            }

            for (auto& [key, vec] : *m_gameObjects) // note the * to dereference
            {
                ImGui::Separator();
                ImGui::Text("Group %zu", key);

                int g_it = 0;
                for (auto& objPtr : vec)
                {
                    GameObject* obj = objPtr.get();
                    if (!obj) continue;

                    // Use the GameObject’s real name accessor
                    const std::string& name = obj->getName();

                    if (ImGui::Button((name + "##" + std::to_string(key) + std::to_string(g_it)).c_str()))
                    {
                        Parameters param;
                        param.PutExtra("Selected", obj);

                        EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_GAMEOBJECT, param);
                    }
                    g_it++;
                }
            }

		}
		ImGui::End();
	}
}

void dx3d::HierarchyUI::setGameObjectList(const std::unordered_map<size_t, std::vector<UniquePtr<GameObject>>>* list)
{
    m_gameObjects = list;
}
