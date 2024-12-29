#include "Renderer.h"
#include "UIContainer.h"

Void CUIContainer::Render(CRenderer* Renderer) {
	Renderer->RenderTexture(Background, Rect);
}