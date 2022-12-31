//
//  File:   EditorApplication.cpp
//  Author: Eugene Kim (komastar.dev@gmail.com)
//
//  Copyright (c) 2022 komastar. All rights reserved.
//

#include "EditorApplication.h"
#include "Application/Private/ApplicationContextFactory.h"
#include "Window/Private/WindowFactory.h"
#include "Graphics/Private/GraphicsDeviceFactory.h"
#include "Graphics/Primitive/WMColor.h"
#include "Graphics/Geometry/WMVertex.h"
#include "Graphics/Geometry/WMGeometryFactory.h"
#include "Math/WMAffineTransform2.h"
#include "Math/WMLinearTransform2.h"

using namespace WildMini;
using namespace WildMini::Common;
using namespace WildMini::Graphics;
using namespace WildMini::Graphics::Primitive;
using namespace WildMini::Graphics::Geometry;
using namespace WildMini::Math;

struct Constants
{
    WMMatrix4 viewProj;
    WMMatrix4 world[3];

};

struct MainPassConstants
{
    WMVector3 eye;
    WMVector3 light;
};

struct ProgressConstants 
{
    WMMatrix4 viewProj;
    WMMatrix4 world[16];
    float ratio[16];
};

EditorApplication::EditorApplication()
    : uiMesh(nullptr)
    , needResize(false)
{
}

EditorApplication::~EditorApplication()
{
}

void EditorApplication::OnInitialize()
{
    window = Window::WindowFactory::Create(1280, 720);
    window->Create();
    window->Show();
    window->Focus();
    window->AddResizeCallback(std::bind(&EditorApplication::OnResize, this, std::placeholders::_1, std::placeholders::_2));

    device = Graphics::Private::GraphicsDeviceFactory::Create();
    commandQueue = device->CreateCommandQueue();
    swapChain = commandQueue->CreateSwapChain(window);

    vertexShader = device->CreateShader(L"Resources/Shader/UIShader.hlsl", "VS", WMShader::StageType::Vertex);
    pixelShader = device->CreateShader(L"Resources/Shader/UIShader.hlsl", "PS", WMShader::StageType::Fragment);

    WMRenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.sampleCount = 1;
    pipelineDesc.vertexShader = vertexShader;
    pipelineDesc.fragmentShader = pixelShader;
    pipelineDesc.vertexDescriptor.attributes = {
        { WMVertexFormat::Float3, "POSITION",       0, 0  },
        { WMVertexFormat::Float2, "TEXCOORD",       0, 12 },
        { WMVertexFormat::Float4, "COLOR",          0, 20 },
        { WMVertexFormat::Uint,   "SV_InstanceID",  0, 28 },
    };

    WMRenderPipelineColorAttachmentDescriptor colorAttach;
    pipelineDesc.colorAttachments = { colorAttach };

    pipelineDesc.depthStencilPixelFormat = WMPixelFormat::DEPTH_24_UNORM_STENCIL_8;
    pipelineDesc.inputPrimitiveTopology = WMPrimitiveTopologyType::Triangle;

    renderPipeline = device->CreateRenderPipeline(pipelineDesc);

    uiCamera.SetView(WMVector3::back, WMVector3::zero, WMVector3::up);
    uiCamera.SetOrthographics(window->GetWidth(), window->GetHeight(), 0.0f, 1000.0f);

    ProgressConstants progressConst;
    progressConst.viewProj = (uiCamera.ViewMatrix() * uiCamera.ProjMatrix()).Transpose();
    progressConst.world[0] = WMMatrix4::Identity();
    progressConst.world[1] = WMMatrix4::Identity();
    progressConst.world[2] = WMMatrix4::Identity();
    progressConst.ratio[0] = 1.0f;
    progressConst.ratio[1] = 0.5f;
    progressConst.ratio[2] = 0.15f;
    progressBuffer = device->CreateGPUBuffer(sizeof(ProgressConstants), WMGPUBuffer::CPUCacheMode::WRITABLE);
    progressBuffer->WriteData(&progressConst, sizeof(ProgressConstants));

    uiMesh = Geometry::WMGeometryFactory::MakeQuad(device, 100.0f, WMColor(1.0f, 0.0f, 0.0f, 0.25f));

    float deltaTime = 0.0f;
    gameThread = WMThread::Create(L"Editor");
    gameThread->Initialize([&]()
        {
            auto begin = std::chrono::high_resolution_clock::now();
            Update(deltaTime);
            Render();
            auto end = std::chrono::high_resolution_clock::now();
            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            deltaTime = dt * 0.001f;
        });
    gameThread->Run();
}

void EditorApplication::OnTerminate()
{
    gameThread->Terminate();
}

void EditorApplication::Update(float dt)
{
    ProgressConstants progressConst;
    progressConst.viewProj = (uiCamera.ViewMatrix() * uiCamera.ProjMatrix()).Transpose();
    progressConst.world[0]._41 = -500.0f;
    progressConst.world[0] = progressConst.world[0].Transpose();
    progressConst.world[1]._41 = -250.0f;
    progressConst.world[1] = progressConst.world[1].Transpose();
    progressConst.world[2]._41 = 250.0f;
    progressConst.world[2] = progressConst.world[0].Transpose();
    progressConst.world[3]._41 = 500.0f;
    progressConst.world[3] = progressConst.world[1].Transpose();
    progressConst.world[4]._41 = 750.0f;
    progressConst.world[4] = progressConst.world[2].Transpose();
    progressBuffer->WriteData(&progressConst, sizeof(ProgressConstants));
}

void EditorApplication::Render()
{
    if (needResize)
    {
        needResize.store(false);
        swapChain->Resize(window->width, window->height);
        uiCamera.SetOrthographics(window->GetWidth(), window->GetHeight(), 0.0f, 1000.0f);
    }

    if (WMSharedPtr<WMCommandBuffer> commandBuffer = commandQueue->CreateCommandBuffer())
    {
        if (WMSharedPtr<WMRenderCommandEncoder> renderCommandEncoder = commandBuffer->CreateRenderCommandEncoder(renderPipeline))
        {
            renderCommandEncoder->ClearRenderTarget(swapChain->RenderTargetTexture(), Primitive::WMColor::black);
            renderCommandEncoder->ClearDepthStencil(swapChain->DepthStencilTexture()
                , WMRenderCommandEncoder::DepthStencilClearFlag::All
                , 0.0f
                , 0);
            Primitive::WMViewport viewport = { 0, 0, window->GetWidth(), window->GetHeight(), 0.0f, 1.0f };
            renderCommandEncoder->SetViewport(viewport);
            Primitive::WMRect scissorRect = { 0, 0, window->GetWidth(), window->GetHeight() };
            renderCommandEncoder->SetScissorRect(scissorRect);
            renderCommandEncoder->SetRenderTargets({ swapChain->RenderTargetTexture() }, swapChain->DepthStencilTexture());
            renderCommandEncoder->SetConstantBuffer(0, progressBuffer);
            renderCommandEncoder->SetVertexBuffer(uiMesh->vertexBuffer, sizeof(WMVertex));
            renderCommandEncoder->DrawPrimitives(WMRenderCommandEncoder::PrimitiveType::Triangle, (uint32_t)uiMesh->vertices.size(), 2, 0, 2);
            //renderCommandEncoder->DrawPrimitives(WMRenderCommandEncoder::PrimitiveType::Triangle, (uint32_t)uiMesh->vertices.size(), 2, 0, 0);
            renderCommandEncoder->ImguiRender();
            renderCommandEncoder->EndEncoding({ swapChain->RenderTargetTexture() });
        }
        commandBuffer->Commit();
    }

    swapChain->Present();
    commandQueue->WaitComplete();
}

void EditorApplication::OnResize(uint32_t width, uint32_t height)
{
    needResize.store(true);
}
