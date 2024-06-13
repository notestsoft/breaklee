#pragma once

#include "UIControl.h"

class CUIEditBox : public CUIControl {
public:
	Bool ObscureText;
	Int32 MaxInputLength;

	using CUIControl::CUIControl;

	virtual Void Render(CRenderer* Renderer);
};
