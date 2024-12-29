#pragma once

#include "Base.h"
#include "UIRect.h"

struct CUITexture {
	CTexture Texture;
    CUIRect Frame;

    CUITexture() : 
        Texture({}),
        Frame(CUIRect()) {
    }

    CUITexture(
        CTexture Texture,
        CUIRect Frame
    ) : Texture(Texture),
        Frame(Frame) {
    }

    CUITexture Slice(
        Int32 X,
        Int32 Y,
        Int32 Width,
        Int32 Height
    ) {
        return CUITexture(Texture, CUIRect(X, Y, X + Width, Y + Height));
    }
};
