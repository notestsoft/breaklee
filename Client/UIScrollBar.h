#pragma once

#include "UIControl.h"

class CUIScrollBar : public CUIControl {
public:
	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
