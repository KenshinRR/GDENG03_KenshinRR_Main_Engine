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
            // Add GameObject button
            if (ImGui::Button("Add New GameObject"))
            {
                ImGui::OpenPopup("GameObjectOptionsPopup");
            }

            if (ImGui::BeginPopup("GameObjectOptionsPopup"))
            {
                Parameters param;
                if (ImGui::MenuItem("Add Empty Game Object"))
                {
                    EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_GAMEOBJECT);
                }
                if (ImGui::MenuItem("Add Bunny"))
                {
                    param.PutExtra("Key", "Bunny");
                    EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_3D_OBJECT, param);
                }
                if (ImGui::MenuItem("Add Armadillo"))
                {
                    param.PutExtra("Key", "Armadillo");
                    EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_3D_OBJECT, param);
                }
                ImGui::EndPopup();
            }


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

                        EventBroadcastManager::getInstance().postEvent(EventNames::ON_GAMEOBJECT_SELECTED, param);
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
