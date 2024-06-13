#pragma once

#include "UIControl.h"

typedef Void* CUISpriteTexture;

class CUISprite : public CUIControl {
public:
	CArray<CUITexture> Textures;

	using CUIControl::CUIControl;

	virtual Void Layout(CUIRect Bounds) {
		CUIControl::Layout(Bounds);
	}

	Void Render(CRenderer* Renderer);
};
