#include "Renderer.h"
#include "UIEditBox.h"

Void CUIEditBox::Update(CUIInputState* InputState) {
	CUIControl::Update(InputState);

	if (State != CUIControlState::Active) return;

    if (InputState->KeyStates[VK_BACK] && !Text.empty()) {
        Text.pop_back();
        InputState->KeyStates[VK_BACK] = false;
    }

    Bool IsFilled = (MaxInputLength > 0 && Text.length() >= MaxInputLength);
    if (!IsFilled) {
        for (auto Key = '0'; Key <= '9'; Key += 1) {
            if (InputState->KeyStates[Key]) {
                Text.push_back(Key);
                InputState->KeyStates[Key] = false;
            }
        }

        for (auto Key = 'A'; Key <= 'Z'; Key += 1) {
            if (InputState->KeyStates[Key]) {
                Bool Capitalized = (
                    InputState->KeyStates[VK_SHIFT] ||
                    InputState->KeyStates[VK_LSHIFT] ||
                    InputState->KeyStates[VK_RSHIFT]
                );
                // bool capsLockActive = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

                Text.push_back(Capitalized ? toupper(Key) : tolower(Key));
                InputState->KeyStates[Key] = false;
            }
        }
    }

    if (InputState->KeyStates[VK_RETURN]) {
        // TODO: Focus and trigger submit button somehow?
    }
}

Void CUIEditBox::Render(CRenderer* Renderer) {
    auto Font = Renderer->GetFont(15, FW_NORMAL, "Arial");

    CUIRect CaretRect = Rect;
    String RenderText = Text;

    if (ObscureText) {
        RenderText = String(RenderText.size(), '*');
    }

    if (!RenderText.empty()) {
        CUIRect TextRect = Renderer->GetTextRect(
            Font,
            RenderText,
            Rect,
            0,
            CUIColor(0xFF, 0xFF, 0xFF, 0xFF)
        );
        
        CaretRect.Left += TextRect.Right - TextRect.Left;
    }

    
    Bool IsCaretVisible = (GetTimestampMs() % (CaretInterval * 2) < CaretInterval);
	if (IsCaretVisible && State == CUIControlState::Active) {
		Renderer->RenderTexture(
			CaretTexture,
            CaretRect
		);
	}

	if (!RenderText.empty()) {
		auto Font = Renderer->GetFont(15, FW_NORMAL, "Arial");

		Renderer->RenderText(
			Font,
            RenderText,
			Rect,
			0,
			CUIColor(0xFF, 0xFF, 0xFF, 0xFF)
		);
	}
}