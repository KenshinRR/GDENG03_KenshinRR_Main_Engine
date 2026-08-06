#pragma once
#include <DX3D/Graphics/GraphicsResource.h>

namespace dx3d
{
	class Texture final : public GraphicsResource
	{
	public:
		Texture(const TextureDesc& desc, const GraphicsResourceDesc& gDesc);
		std::string getName() { return m_name; }
		void setName(std::string name) { m_name = name; }
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv{};
		friend class DeviceContext;
		std::string m_name;
	};
}