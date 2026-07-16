#pragma once
#include <DX3D/UI/BaseUI.h>
#include <imgui.h>

namespace dx3d
{

	class SceneUI : public BaseUI
	{
	public:
		SceneUI(const BaseDesc& desc);
		void draw() override;

		~SceneUI();

		void setLogo(RefPtr<TextureResource> logoTexture);
	private:
		void drawAboutTabItem();

	private:
		bool m_showCredits = false;
		bool m_showColorPicker = false;
		float* m_color; // RGBA
		ImTextureID m_logo;
	};

}