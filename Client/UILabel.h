#pragma once

#include "UIControl.h"

class CUILabel : public CUIControl {
public:
	String Text;
	CUIColor Color;

	using CUIControl::CUIControl;
};
