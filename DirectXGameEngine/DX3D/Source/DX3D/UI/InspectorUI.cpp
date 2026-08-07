#include <DX3D/UI/InspectorUI.h>
#include <DX3D/Game/GameObject.h>

#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <DX3D/EventBroadcasting/Parameters.h>

#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/MeshComponent.h>

#include <DX3D/Physics/PhysicsManager.h>

dx3d::InspectorUI::InspectorUI(const BaseDesc& desc) : BaseUI(desc)
{
	EventBroadcastManager::getInstance().addObserver
	(
		EventNames::ON_GAMEOBJECT_SELECTED,
		[this](dx3d::Parameters& params)
		{
			m_selectedGameObject = params.GetGameObjectPtr("Selected", NULL);
		}
	);
}

dx3d::InspectorUI::~InspectorUI()
{
	EventBroadcastManager::getInstance().RemoveObserver(EventNames::ON_GAMEOBJECT_SELECTED);
}

void dx3d::InspectorUI::draw()
{
	if (m_showInspector)
	{
		if (ImGui::Begin("Inspector", &m_showInspector))
		{
			// Make sure we actually have a list set
			if (!m_selectedGameObject || m_selectedGameObject == NULL)
			{
				ImGui::Text("No game object selected.");
				ImGui::End();
				return;
			}

			// Add GameObject button
			if (ImGui::Button("Add New Component"))
			{
				ImGui::OpenPopup("ComponentOptionsPopup");
			}

			if (ImGui::BeginPopup("ComponentOptionsPopup"))
			{
				if (ImGui::MenuItem("Add Mesh Component"))
				{
					m_selectedGameObject->createOrGetComponent<dx3d::MeshComponent>();
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginTabBar("##INSPECTOR")) // create tab bar with id
			{
				if (ImGui::BeginTabItem("Transform"))
				{
					drawTransformInspector();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

void dx3d::InspectorUI::drawTransformInspector()
{
	auto& transform = m_selectedGameObject->getTransform();

	auto pos = transform.getPosition();
	auto rot = transform.getRotation();
	auto scale = transform.getScale();

	float p[3] = { pos.x, pos.y, pos.z };
	float r[3] = { rot.x, rot.y, rot.z };
	float s[3] = { scale.x, scale.y, scale.z };

	// Position
	if (ImGui::DragFloat3("Position", p, 0.05f, -FLT_MAX, FLT_MAX, "%.3f"))
	{
		transform.setPosition({ p[0], p[1], p[2] });
	}

	// Rotation 
	if (ImGui::DragFloat3("Rotation (rad)", r, 0.01f, -FLT_MAX, FLT_MAX, "%.3f"))
	{
		transform.setRotation({ r[0], r[1], r[2] });
	}

	// Scale
	if (ImGui::DragFloat3("Scale", s, 0.01f, 0.0f, FLT_MAX, "%.3f"))
	{
		transform.setScale({ s[0], s[1], s[2] });
	}

	PhysicsManager::getInstance().syncComponentToPhysics(&transform);
}
