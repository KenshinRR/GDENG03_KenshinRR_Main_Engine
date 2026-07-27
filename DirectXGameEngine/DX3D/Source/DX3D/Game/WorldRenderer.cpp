#include <DX3D/Game/WorldRenderer.h>
#include <DX3D/Game/Display.h>

#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/RenderSystem/DeviceContext/DeviceContext.h>
#include <DX3D/Graphics/RenderSystem/SwapChain/SwapChain.h>
#include <DX3D/Graphics/RenderSystem/VertexBuffer/VertexBuffer.h>
#include <DX3D/Graphics/RenderSystem/IndexBuffer/IndexBuffer.h>

#include <DX3D/Game/World.h>
#include <DX3D/Game/Component.h>
#include <DX3D/Game/GameObject.h>

#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CubeComponent.h>
#include <DX3D/Component/CameraComponent.h>
#include <DX3D/Component/MeshComponent.h>

#include <DX3D/Resource/MaterialResource.h>
#include <DX3D/Resource/TextureResource.h>

#include <DX3D/Math/Vec3.h>
#include <fstream>
#include <ranges>

// ADDED: DirectX 11 backend renders ImGui after the engine command list has been executed.
#include <imgui_impl_dx11.h>

// event broadcast needed
#include <DX3D/EventBroadcasting/EventBroadcastManager.h>
#include <DX3D/EventBroadcasting/EventNames.h>
#include <iostream>

#include <d3d11.h>
#include <dxgi.h>

dx3d::WorldRenderer::WorldRenderer(const WorldRendererDesc& desc) : Base(desc.base), m_graphicsDevice(desc.engine)
{
	auto& device = m_graphicsDevice;
	m_deviceContext = device.createDeviceContext();

	//m_cb = device.createConstantBuffer({ {}, sizeof(ConstantData) });

	m_textures.reserve(32);

	m_objectCb = device.createConstantBuffer({ {}, sizeof(ObjectData) });
	m_cameraCb = device.createConstantBuffer({ {}, sizeof(CameraData) });
	m_materialCb = device.createConstantBuffer({ {}, dx3d::MaterialResource::MaxDataSize });
	m_lightCb = device.createConstantBuffer({ {}, sizeof(LightData) });

	m_sampler = device.createSampler({});



	// create one wireframe rasterizer; this might not work but am gonna test
	dx3d::RasterizerDesc rd{};
	rd.isWire = true;
	m_rasterizer = m_graphicsDevice.createRasterizer(rd);

	// register for wireframe toggle events
	dx3d::EventBroadcastManager::getInstance().addObserver(dx3d::EventNames::WIREFRAME_TOGGLE, [this]()
		{
		if(wireToggle)
		{
			wireToggle = false;
		}
		else
		{
			wireToggle = true;
		}
		std::cout << "Wireframe mode toggled: " << (wireToggle ? "ON" : "OFF") << std::endl;
		
		});
}

void dx3d::WorldRenderer::render(const World& world, SwapChain& swapChain, f32 deltaTime)
{
	auto size = swapChain.getSize();

	auto& context = *m_deviceContext;
	context.clearAndSetBackBuffer(swapChain, { 0.27f, 0.39f, 0.55f, 1.0f });
	context.setViewportSize(size);

	Sampler* samplers[] = { m_sampler.get() };
	context.setSamplers(std::span<Sampler*>{samplers});

	auto numComponents = 0u;

	auto& cameraCb = *m_cameraCb;
	auto& objectCb = *m_objectCb;
	auto& materialCb = *m_materialCb;

	{
		CameraData cameraData{};
		auto components = world.getComponents<CameraComponent>(numComponents);
		for (auto i : std::views::iota(0u, numComponents))
		{

			auto component = components[i];
			cameraData.view = component->getViewMatrix();
			component->setViewportSize(size);
			cameraData.proj = component->getProjectionMatrix();
			cameraData.cameraPosition = Vec4(
				component->getGameObject().getTransform().getPosition().x,
				component->getGameObject().getTransform().getPosition().y,
				component->getGameObject().getTransform().getPosition().z,
				1.0f
			);
			context.updateConstantBuffer(cameraCb, std::as_bytes(std::span{ &cameraData, 1 }));

			if (wireToggle && m_rasterizer) context.setRasterizerState(*m_rasterizer);
			else context.clearRaster();
			break;
		}
	}

	{
		LightData lightData{};
		lightData.lightDirection = Vec4(0.577f, -0.577f, 0.577f, 0.0f);
		lightData.lightColor = Vec4(1.0f, 0.95f, 0.9f, 1.0f);
		lightData.ambientColor = Vec4(0.2f, 0.22f, 0.25f, 1.0f);
		context.updateConstantBuffer(*m_lightCb, std::as_bytes(std::span{ &lightData, 1 }));
	}

	// Render all MeshComponents
	{
		ObjectData objectData{};
		auto components = world.getComponents<MeshComponent>(numComponents);

		for (auto i : std::views::iota(0u, numComponents))
		{
			auto component = components[i];
			auto& transform = component->getGameObject().getTransform();
			auto mesh = component->getMesh();

			auto material = component->getMaterial();

			if (material)
			{
				objectData.world = transform.getAffineWorldMatrix();

				context.setGraphicsPipelineState(material->getGraphicsPipelineState());
				context.updateConstantBuffer(objectCb, std::as_bytes(std::span{ &objectData, 1 }));
				context.updateConstantBuffer(materialCb, material->getData());
				ConstantBuffer* cbs[] = { &objectCb, &cameraCb, &materialCb, m_lightCb.get() };
				context.setConstantBuffers(std::span<ConstantBuffer*>{cbs});

				auto vb = component->getOrCreateVertexBuffer(m_graphicsDevice);
				auto ib = component->getOrCreateIndexBuffer(m_graphicsDevice);

				/*context.setVertexBuffer(*vb);
				context.setIndexBuffer(*ib);
				context.drawIndexedTriangleList(mesh->getIndexCount(), 0u, 0u);*/

				m_textures.clear();
				m_textures.resize(material->getNumTextures());
				for (auto t : std::views::iota(0u, m_textures.size()))
				{
					auto tex = material->getTexture(t);
					if (tex) m_textures[t] = &tex->getTexture();
				}
				context.setTextures(std::span<Texture*>{m_textures});

				context.setVertexBuffer(*vb);
				context.setIndexBuffer(*ib);
				context.drawIndexedTriangleList(mesh->getIndexCount(), 0u, 0u);
			}
		}

		m_graphicsDevice.executeCommandList(context);
		swapChain.present();
	}
}

void dx3d::WorldRenderer::renderWorldViewport(const World& world)
{
	ImGui::Begin("World View");
	ImVec2 avail = ImGui::GetContentRegionAvail();

	// Recreate offscreen target if size changed and valid
	static int texW = 0, texH = 0;
	int newW = (int)avail.x;
	int newH = (int)avail.y;

	if (newW > 0 && newH > 0 && (newW != texW || newH != texH)) {
		texW = newW;
		texH = newH;
		createOffscreenTarget(texW, texH);
	}

	// Only render if we have a valid target
	if (texW > 0 && texH > 0 && m_offscreenSRV) {
		renderToTexture(world, texW, texH);
		ImGui::Image((ImTextureID)m_offscreenSRV.Get(), avail);
	}

	// Show texture
	ImGui::Image((ImTextureID)m_offscreenSRV.Get(), avail);

	ImGui::End();

}

void dx3d::WorldRenderer::renderForDisplay(const World& world, Display& display, f32 deltaTime, ImDrawData* uiDrawData)
{
	HWND hwnd = static_cast<HWND>(display.getHandle());
	if (IsIconic(hwnd))
		return;

	auto& swapChain = display.getSwapChain();
	auto size = swapChain.getSize();

	auto& context = *m_deviceContext;
	context.clearAndSetBackBuffer(swapChain, { 0.27f, 0.39f, 0.55f, 1.0f });
	context.setViewportSize(size);

	Sampler* samplers[] = { m_sampler.get() };
	context.setSamplers(std::span<Sampler*>{samplers});

	if (auto* camera = display.getCamera())
	{
		CameraData cameraData{};
		cameraData.view = camera->getViewMatrix();
		camera->setViewportSize(size);
		cameraData.proj = camera->getProjectionMatrix();
		cameraData.cameraPosition = Vec4(
			camera->getGameObject().getTransform().getPosition().x,
			camera->getGameObject().getTransform().getPosition().y,
			camera->getGameObject().getTransform().getPosition().z,
			1.0f
		);
		context.updateConstantBuffer(*m_cameraCb, std::as_bytes(std::span{ &cameraData, 1 }));
	}

	if (display.getRenderMode() == Display::RenderMode::Wireframe && m_rasterizer) context.setRasterizerState(*m_rasterizer);
	else context.clearRaster();

	{
		LightData lightData{};
		lightData.lightDirection = Vec4(0.577f, -0.577f, 0.577f, 0.0f);
		lightData.lightColor = Vec4(1.0f, 0.95f, 0.9f, 1.0f);
		lightData.ambientColor = Vec4(0.2f, 0.22f, 0.25f, 1.0f);
		context.updateConstantBuffer(*m_lightCb, std::as_bytes(std::span{ &lightData, 1 }));
	}

	ui32 numComponents = 0;
	auto components = world.getComponents<MeshComponent>(numComponents);
	for (auto i : std::views::iota(0u, numComponents))
	{
		auto component = components[i];
		auto& transform = component->getGameObject().getTransform();
		auto mesh = component->getMesh();
		auto material = component->getMaterial();

		if (material)
		{
			ObjectData objectData{};
			objectData.world = transform.getAffineWorldMatrix();

			context.setGraphicsPipelineState(material->getGraphicsPipelineState());
			context.updateConstantBuffer(*m_objectCb, std::as_bytes(std::span{ &objectData, 1 }));
			context.updateConstantBuffer(*m_materialCb, material->getData());
			ConstantBuffer* cbs[] = { m_objectCb.get(), m_cameraCb.get(), m_materialCb.get(), m_lightCb.get() };
			context.setConstantBuffers(std::span<ConstantBuffer*>{cbs});

			auto vb = component->getOrCreateVertexBuffer(m_graphicsDevice);
			auto ib = component->getOrCreateIndexBuffer(m_graphicsDevice);

			m_textures.clear();
			m_textures.resize(material->getNumTextures());
			for (auto t : std::views::iota(0u, m_textures.size()))
			{
				auto tex = material->getTexture(t);
				if (tex) m_textures[t] = &tex->getTexture();
			}
			context.setTextures(std::span<Texture*>{m_textures});

			context.setVertexBuffer(*vb);
			context.setIndexBuffer(*ib);
			context.drawIndexedTriangleList(mesh->getIndexCount(), 0u, 0u);
		}
	}

	m_graphicsDevice.executeCommandList(context);
	if (uiDrawData)
	{
		auto* renderTarget = swapChain.getRenderTargetView();
		m_graphicsDevice.getNativeContext()->OMSetRenderTargets(1, &renderTarget, nullptr);
		ImGui_ImplDX11_RenderDrawData(uiDrawData);
	}
	swapChain.present();
}

void dx3d::WorldRenderer::renderToTexture(const World& world, int width, int height)
{
	auto& context = *m_deviceContext;
	context.getNativeContext()->OMSetRenderTargets(1, m_offscreenRTV.GetAddressOf(), nullptr);

	const float clearColor[4] = { 0.27f, 0.39f, 0.55f, 1.0f };
	context.getNativeContext()->ClearRenderTargetView(m_offscreenRTV.Get(), clearColor);

	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context.getNativeContext()->RSSetViewports(1, &vp);
}

void dx3d::WorldRenderer::createOffscreenTarget(int width, int height)
{
	auto* device = m_graphicsDevice.getNativeDevice();
	if (!device) {
		DX3DLogThrowError("Native Device Not Initialized!");
		return;
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &m_offscreenTex);
	if (FAILED(hr)) {
		DX3DLogError("CreateTexture2D failed!\n");
		return;
	}

	hr = device->CreateRenderTargetView(m_offscreenTex.Get(), nullptr, &m_offscreenRTV);
	if (FAILED(hr)) {
		DX3DLogError("CreateRenderTargetView failed!\n");
		return;
	}

	// Explicit SRV descriptor
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(m_offscreenTex.Get(), &srvDesc, &m_offscreenSRV);
	if (FAILED(hr)) {
		DX3DLogError("CreateShaderResourceView failed!\n");
		return;
	}


}
