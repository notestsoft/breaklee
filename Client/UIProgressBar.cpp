#include "Renderer.h"
#include "UIProgressBar.h"

Void CUIProgressBar::Render(CRenderer* Renderer) {
	Renderer->RenderTexture(Bar, Rect);
}