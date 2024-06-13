#include "Renderer.h"
#include "UISprite.h"

Void CUISprite::Render(CRenderer* Renderer) {
	for (auto& Texture : Textures) {
		Renderer->RenderTexture(Texture, Rect);
	}
}