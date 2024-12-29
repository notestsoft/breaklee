#pragma once

#include "UIControl.h"

class CUITextBox : public CUIControl {
public:
	String Text;
	Bool Bold;
	UInt32 Format;
	CMap<CUIControlState, CUIColor> Colors;

	using CUIControl::CUIControl;

	Void Render(CRenderer* Renderer);
};
