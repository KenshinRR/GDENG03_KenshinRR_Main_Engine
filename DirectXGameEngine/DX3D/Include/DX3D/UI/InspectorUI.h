#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/UI/BaseUI.h>
#include <DX3D/Math/Vec3.h>
#include <vector>

namespace dx3d
{

	class InspectorUI : public BaseUI
	{
	public:
		InspectorUI(const BaseDesc& desc);
		void draw() override;

		~InspectorUI();
	private:
		void render(World& world, GraphicsDevice& graphicsDevice, SwapChain& swapChain);
		void drawGameObjectPanel(World& world);
		void drawTransformInspector(GameObject& object);
		void drawComponentInspector(GameObject& object);
		void drawViewportPanel(Display& display);
	private:
		GameObject* m_selectedGameObject{};
		bool m_isPlayMode{ false };
		bool m_trackingPositionEdit{ false };
		bool m_trackingRotationEdit{ false };
		bool m_trackingScaleEdit{ false };
		Vec3 m_editStartPosition{};
		Vec3 m_editStartRotation{};
		Vec3 m_editStartScale{};
		//Display& m_display;
	};

}
