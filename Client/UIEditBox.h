#pragma once

#include "UIControl.h"

class CUIEditBox : public CUIControl {
public:
	CUITexture CaretTexture;
	CUITexture UnknownTexture1;
	CUITexture UnknownTexture2;
	CUITexture UnknownTexture3;

	Bool ObscureText;
	Int32 MaxInputLength;
	String Text;
	Int32 CaretInterval = 500;

	using CUIControl::CUIControl;

	virtual Void Update(CUIInputState* InputState);

	virtual Void Render(CRenderer* Renderer);
};
