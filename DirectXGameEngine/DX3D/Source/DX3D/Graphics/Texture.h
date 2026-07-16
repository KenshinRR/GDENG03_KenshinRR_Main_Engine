#pragma once
#include <DX3D/Graphics/GraphicsResource.h>

namespace dx3d
{
	class Texture final : public GraphicsResource
	{
	public:
		Texture(const TextureDesc& desc, const GraphicsResourceDesc& gDesc);
		ID3D11ShaderResourceView* getSRV() const { return m_srv.Get(); }
		ID3D11Texture2D* getTexture2D() const { return m_texture.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv{};
		friend class DeviceContext;
	};
}