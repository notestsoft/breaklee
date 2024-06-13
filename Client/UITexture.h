#pragma once

#include "Base.h"
#include "UIRect.h"

struct CUITexture {
    Void* Handle;
    CUIRect Frame;

    CUITexture() : 
        Handle(nullptr),
        Frame(CUIRect()) {
    }

    CUITexture(
        Void* Handle,
        CUIRect Frame
    ) : Handle(Handle),
        Frame(Frame) {
    }
};
