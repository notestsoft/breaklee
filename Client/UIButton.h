#pragma once

#include "UIControl.h"
#include "UITexture.h"

class CUIButton : public CUIControl {
public:
	CMap<CUIControlState, CUITexture> Textures;

	using CUIControl::CUIControl;

	virtual Void Update(CUIInputState* InputState) {
		if (!InputState->KeyStates[VK_LBUTTON] && State == CUIControlState::Active) {
			// TriggerEvent
		}

		CUIControl::Update(InputState);
	}

	virtual Void Render(CRenderer* Renderer);
};
