#pragma once
#include <DX3D/All.h>
#include <DX3D/UI/BaseUI.h>
#include <vector>


class MainGame : public dx3d::Game
{
public:
	explicit MainGame(const dx3d::GameDesc& desc);
protected:
	virtual void onCreate();
	virtual void onUpdate(dx3d::f32 deltaTime);
	virtual void onDrawUi() override;
private:
	//dx3d::UniquePtr<dx3d::SceneUI> m_SceneUI{};
	std::vector< dx3d::UniquePtr<dx3d::BaseUI>> m_UIs{};
};