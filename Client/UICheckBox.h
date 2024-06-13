#pragma once

#include "UIControl.h"

class CUICheckBox : public CUIControl {
public:
	CMap<CUIControlState, CUITexture> Textures;

	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
