#pragma once

#include "UIControl.h"

class CUITabPanel : public CUIControl {
public:
	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
