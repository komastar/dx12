//
//  File:   WMCommandBuffer.h
//  Author: Eugene Kim (komastar.dev@gmail.com)
//
//  Copyright (c) 2022 komastar. All rights reserved.
//

#pragma once
#include "WMFramework.h"
#include "WMSharedPtr.h"
#include "Graphics/WMRenderCommandEncoder.h"
#include "Graphics/WMRenderPipeline.h"

namespace WildMini::Graphics
{
    class WM_API WMCommandBuffer : public WildMini::WMRefCounter
    {
    public:
        virtual WildMini::WMSharedPtr<WMRenderCommandEncoder> CreateRenderCommandEncoder(WMRenderPipeline* renderPipeline) = 0;
        virtual void Commit() = 0;
    };
}
