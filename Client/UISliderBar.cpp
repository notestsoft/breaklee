#include "Renderer.h"
#include "UISliderBar.h"

Void CUISliderBar::Update(CUIInputState* InputState) {

	/*
	if (InputState->KeyStates[VK_LBUTTON] && State == CUIControlState::Active) {
		auto& BarPair = StateToBarTexture[State];
		auto MinX = LayoutRect.Left;
		auto MaxX = LayoutRect.Left + BarPair.second.Right - BarPair.second.Left;
		auto TrackX = MAX(MinX, MIN(MaxX, InputState->MouseX));
		TrackValue = (Float32)(TrackX - MinX) / (MaxX - MinX);
	}
	*/
	CUIControl::Update(InputState);
}

Void CUISliderBar::Render(CRenderer* Renderer) {
	Renderer->RenderTexture(Bars[State], Rect);
	Renderer->RenderTexture(Knobs[State], Rect);
}