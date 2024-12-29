#pragma once

#include "UIControl.h"
#include "UITexture.h"

class CUIButton : public CUIControl {
public:
	CMap<CUIControlState, CUITexture> Textures;

	using CUIControl::CUIControl;

	virtual Void Update(CUIInputState* InputState);

	virtual Void Render(CRenderer* Renderer);
};
