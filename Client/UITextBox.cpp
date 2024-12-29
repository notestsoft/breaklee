#include "Renderer.h"
#include "UITextBox.h"

Void CUITextBox::Render(CRenderer* Renderer) {
	if (!Text.empty()) {
		auto Font = Renderer->GetFont(15, Bold ? FW_BOLD : FW_NORMAL, "Arial");

		Renderer->RenderText(
			Font,
			Text,
			Rect,
			Format,
			Colors[State]
		);
	}
}