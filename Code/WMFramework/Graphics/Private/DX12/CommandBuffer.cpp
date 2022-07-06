//
//  File:   CommandBuffer.cpp
//  Author: Eugene Kim (komastar.dev@gmail.com)
//
//  Copyright (c) 2022 komastar. All rights reserved.
//

#include "CommandBuffer.h"
#include "RenderPipeline.h"

using namespace WildMini::Object;
using namespace WildMini::Graphics;
using namespace WildMini::Graphics::Private::DX12;

CommandBuffer::CommandBuffer(CommandQueue* _commandQueue, ID3D12CommandAllocator* _commandAllocator, ID3D12GraphicsCommandList* _commandList, D3D12_COMMAND_LIST_TYPE _type)
    : type(_type)
    , commandQueue(_commandQueue)
    , commandList(_commandList)
    , commandAllocator(_commandAllocator)
{
}

WMObject<WMRenderCommandEncoder> CommandBuffer::CreateRenderCommandEncoder(WMRenderPipeline* _renderPipeline)
{
    RenderPipeline* renderPipeline = dynamic_cast<RenderPipeline*>(_renderPipeline);
    commandList->Reset(commandAllocator.Get(), renderPipeline->PipelineState());
    return new RenderCommandEncoder(renderPipeline, this, commandList.Get());
}

void CommandBuffer::Commit()
{
    ID3D12CommandList* cmdList[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, cmdList);
}

void CommandBuffer::AddEncodedCommandList(ID3D12GraphicsCommandList* _commandList)
{
    commandList = _commandList;
}
