#include <DX3D/UI/HierarchyUI.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <DX3D/EventBroadcasting/Parameters.h>

dx3d::HierarchyUI::HierarchyUI(const BaseDesc& desc) : BaseUI(desc)
{
    EventBroadcastManager::getInstance().addObserver(
        EventNames::ON_EDITOR_PLAY_MODE_CHANGED,
        [this](Parameters& params)
        {
            m_isPlayMode = params.GetBoolExtra("IsPlayMode", false);
        }
    );
}

dx3d::HierarchyUI::~HierarchyUI()
{
    EventBroadcastManager::getInstance().RemoveObserver(EventNames::ON_EDITOR_PLAY_MODE_CHANGED);
}

void dx3d::HierarchyUI::draw()
{
    if (m_showHierarchy)
    {
        if (ImGui::Begin("Hierarchy", &m_showHierarchy))
        {
            ImGui::TextColored(
                m_isPlayMode ? ImVec4(1.0f, 0.55f, 0.25f, 1.0f) : ImVec4(0.25f, 0.9f, 0.45f, 1.0f),
                m_isPlayMode ? "Mode: Play (scene editing locked)" : "Mode: Edit"
            );
            ImGui::Separator();

            ImGui::BeginDisabled(m_isPlayMode);

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
                    EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_EMPTY_GAMEOBJECT);
                }
                if (ImGui::MenuItem("Add Cube"))
                {
                    param.PutExtra("Key", "Cube");
                    EventBroadcastManager::getInstance().postEvent(EventNames::ON_ADD_3D_OBJECT, param);
					DX3DLogInfo("Add Cube GameObject event posted.");
                }
                ImGui::EndPopup();
            }

            ImGui::EndDisabled();


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
                    if (!obj || obj->isDeleted()) continue;

                    // Use the GameObject’s real name accessor
                    const std::string& name = obj->getName();

                    ImGui::PushID(obj);

                    bool enabled = obj->isEnabled();
                    ImGui::BeginDisabled(m_isPlayMode);
                    if (ImGui::Checkbox("##Enabled", &enabled))
                    {
                        Parameters param;
                        param.PutExtra("Target", obj);
                        param.PutExtra("Enabled", enabled);
                        EventBroadcastManager::getInstance().postEvent(EventNames::ON_SET_GAMEOBJECT_ENABLED, param);
                    }
                    ImGui::EndDisabled();

                    ImGui::SameLine();
                    if (ImGui::SmallButton(("Select##" + std::to_string(g_it)).c_str()))
                    {
                        Parameters param;
                        param.PutExtra("Selected", obj);

                        EventBroadcastManager::getInstance().postEvent(EventNames::ON_GAMEOBJECT_SELECTED, param);
                    }

                    ImGui::SameLine();
                    ImGui::TextUnformatted(name.c_str());

                    ImGui::SameLine();
                    ImGui::BeginDisabled(m_isPlayMode);
                    if (ImGui::SmallButton("Delete"))
                    {
                        Parameters param;
                        param.PutExtra("Target", obj);
                        EventBroadcastManager::getInstance().postEvent(EventNames::ON_DELETE_GAMEOBJECT, param);
                    }
                    ImGui::EndDisabled();

                    ImGui::PopID();
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

