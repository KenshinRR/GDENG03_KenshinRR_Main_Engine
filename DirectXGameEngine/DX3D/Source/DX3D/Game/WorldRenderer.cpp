#include <DX3D/Game/WorldRenderer.h>
#include <DX3D/Graphics/GraphicsDevice.h>
#include <DX3D/Graphics/RenderSystem/DeviceContext/DeviceContext.h>
#include <DX3D/Graphics/RenderSystem/SwapChain/SwapChain.h>
#include <DX3D/Graphics/RenderSystem/VertexBuffer/VertexBuffer.h>
#include <DX3D/Graphics/RenderSystem/IndexBuffer/IndexBuffer.h>

#include <DX3D/Scene/SceneManager.h>
#include <DX3D/Scene/Scene.h>
#include <DX3D/Game/GameObject.h>
#include <DX3D/Game/Component.h>

#include <DX3D/Component/TransformComponent.h>
#include <DX3D/Component/CameraComponent.h>
#include <DX3D/Component/MeshComponent.h>

#include <DX3D/Resource/MaterialResource.h>
#include <DX3D/Resource/TextureResource.h>

#include <imgui_impl_dx11.h>
#include <ranges>

dx3d::WorldRenderer::WorldRenderer(const WorldRendererDesc& desc)
    : Base(desc.base), m_graphicsDevice(desc.engine)
{
    auto& device = m_graphicsDevice;
    m_deviceContext = device.createDeviceContext();

    m_textures.reserve(32);

    m_objectCb = device.createConstantBuffer({ {}, sizeof(ObjectData) });
    m_cameraCb = device.createConstantBuffer({ {}, sizeof(CameraData) });
    m_materialCb = device.createConstantBuffer({ {}, dx3d::MaterialResource::MaxDataSize });

    m_sampler = device.createSampler({});
}

void dx3d::WorldRenderer::render(SwapChain& swapChain, f32 deltaTime, ImDrawData* uiDrawData, const Display* uiDisplay)
{
    Scene* activeScene = SceneManager::getInstance().getActiveScene();
    if (!activeScene) return; // nothing to render

    auto size = swapChain.getSize();
    auto& context = *m_deviceContext;

    context.clearAndSetBackBuffer(swapChain, { 0.27f, 0.39f, 0.55f, 1.0f });
    context.setViewportSize(size);

    Sampler* samplers[] = { m_sampler.get() };
    context.setSamplers(std::span<Sampler*>{samplers});

    auto& cameraCb = *m_cameraCb;
    auto& objectCb = *m_objectCb;
    auto& materialCb = *m_materialCb;

    // Camera setup
    {
        CameraData cameraData{};
        ui32 numComponents = 0u;
        auto components = reinterpret_cast<CameraComponent* const*>(
            activeScene->getComponentsInternal(CameraComponent::GetTypeId(), &numComponents)
            );

        for (auto i : std::views::iota(0u, numComponents))
        {
            auto component = components[i];
            cameraData.view = component->getViewMatrix();
            component->setViewportSize(size);
            cameraData.proj = component->getProjectionMatrix();
            context.updateConstantBuffer(cameraCb, std::as_bytes(std::span{ &cameraData, 1 }));
            break; // use first camera
        }
    }

    // Render all MeshComponents
    {
        ObjectData objectData{};
        ui32 numComponents = 0u;
        auto components = reinterpret_cast<MeshComponent* const*>(
            activeScene->getComponentsInternal(MeshComponent::GetTypeId(), &numComponents)
            );

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
                ConstantBuffer* cbs[] = { &objectCb, &cameraCb, &materialCb };
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

        auto* renderTarget = swapChain.getRenderTargetView();
        m_graphicsDevice.getNativeContext()->OMSetRenderTargets(1, &renderTarget, nullptr);

        ImGui_ImplDX11_RenderDrawData(uiDrawData);
        swapChain.present();
    }
}
