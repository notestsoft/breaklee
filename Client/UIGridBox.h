#pragma once

#include "UISprite.h"

class CUIGridBox : public CUISprite {
public:
	using CUISprite::CUISprite;

	virtual Void Render(CRenderer* Renderer);
};
