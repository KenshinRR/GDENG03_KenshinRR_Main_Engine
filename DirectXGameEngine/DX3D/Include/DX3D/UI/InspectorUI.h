#pragma once
#include <DX3D/UI/BaseUI.h>
#include <DX3D/Game/GameObject.h>
#include <imgui.h>

namespace dx3d
{

	class InspectorUI : public BaseUI
	{
	public:
		InspectorUI(const BaseDesc& desc);
		~InspectorUI();

		void draw() override;

	private:
		void drawTransformInspector();

	private:
		bool m_showInspector = true;
		GameObject* m_selectedGameObject;
	};

}