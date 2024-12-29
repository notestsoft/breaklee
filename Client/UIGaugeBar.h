#pragma once

#include "UIControl.h"

class CUIGaugeBar : public CUIControl {
public:
	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
