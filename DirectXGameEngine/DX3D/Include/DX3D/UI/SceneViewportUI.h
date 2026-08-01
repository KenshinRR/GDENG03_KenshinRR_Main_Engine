#pragma once
#include <DX3D/UI/BaseUI.h>
#include <imgui.h>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

namespace dx3d
{

	class SceneViewportUI : public BaseUI
	{
	public:
		SceneViewportUI(const BaseDesc& desc, World& world, WorldRenderer& worldRenderer);
		~SceneViewportUI();

		void draw() override;

		void renderWorldViewport();
		void renderToTexture(int width, int height);
		void createOffscreenTarget(int width, int height);
		void renderScene(int width, int height);

		void setName(std::string name) { m_name = name; }
		void setID(size_t id) { m_camera_ID = id; }
	private:
		std::string m_name = "Scene View";
		size_t m_camera_ID = 0;
		const World& m_world;
		bool m_showHierarchy = true;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_offscreenTex;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_offscreenRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_offscreenSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_offscreenDSV;

		RefPtr<ConstantBuffer> m_cameraCb{};
		RefPtr<ConstantBuffer> m_objectCb{};
		RefPtr<ConstantBuffer> m_materialCb{};
		RefPtr<ConstantBuffer> m_lightCb{};

		GraphicsDevice& m_graphicsDevice;
		RefPtr<DeviceContext> m_deviceContext{};

		std::vector<Texture*> m_textures{};
	};

}
