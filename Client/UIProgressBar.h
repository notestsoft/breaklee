#pragma once

#include "UIControl.h"

class CUIProgressBar : public CUIControl {
public:
	CUITexture Bar;
	Float32 Progress;

	using CUIControl::CUIControl;

	Void Update(CUIInputState* InputState) {
		Progress += 1.0f / 60;
		if (Progress >= 1.0f) {
			Progress = 0.0f;
		}

		CUIControl::Update(InputState);
	}

	Void Render(CRenderer* Renderer);
};
