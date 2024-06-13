#pragma once

#include "UISprite.h"

class CUISliderBar : public CUIControl {
public:
	CMap<CUIControlState, CUITexture> Bars;
	CMap<CUIControlState, CUITexture> Knobs;
	Float32 TrackValue;

	using CUIControl::CUIControl;

	Void Update(CUIInputState* InputState);

	Void Render(CRenderer* Renderer);
};
