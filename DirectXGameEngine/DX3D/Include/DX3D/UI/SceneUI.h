#pragma once
#include <DX3D/Core/Base.h>
#include <DX3D/UI/BaseUI.h>

namespace dx3d
{

	class GraphicsDevice;
	class SwapChain;
	class World;
	class GameObject;

	class SceneUI : public BaseUI
	{
	public:
		SceneUI(const BaseDesc& desc);
		void draw() override;

		~SceneUI();
	private:
		void drawAboutTabItem();
	};

}