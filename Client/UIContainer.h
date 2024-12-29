#pragma once

#include "UIControl.h"
#include "UITexture.h"

class CUIContainer : public CUIControl {
public:
	CUITexture Background;

	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
