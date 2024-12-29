#include "Renderer.h"
#include "UIButton.h"

Void CUIButton::Update(CUIInputState* InputState) {
	if (!InputState->KeyStates[VK_LBUTTON] && State == CUIControlState::Active) {
		State = CUIControlState::Normal;

		for (auto Event : Element->Events) {
			UIManager->TriggerEvent(Element, Event);
		}
		// TriggerEvent
	}

	CUIControl::Update(InputState);
}

Void CUIButton::Render(CRenderer* Renderer) {
	Renderer->RenderTexture(Textures[State], Rect);
}