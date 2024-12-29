#pragma once

#include "Base.h"
#include "UIPoint.h"
#include "UIManager.h"

class CFont {
	friend class CRenderer;

	Int32 FontSize;
	Int32 FontWeight;
	String FontName;
	LPD3DXFONT Handle;

	CFont(
		Int32 FontSize,
		Int32 FontWeight,
		String FontName,
		LPD3DXFONT Handle
	);

public:
	CFont();
};

class CRenderer {
	friend class CApplication;

	LPDIRECT3D9 Driver;
	LPDIRECT3DDEVICE9 Device;
	LPD3DXSPRITE Sprite;
	CMap<String, CFont> Fonts;
	Size Resolution;
	UIColor ClearColor;

	CRenderer(
		HWND Window
	);

	~CRenderer();

	Void Render();

	Void BeginSceneUI();

	Void EndSceneUI();

public:
	D3DXMATRIX TransformMatrix;

	CUIRect GetBounds();

	CTexture LoadTexture(
		String FilePath,
		Bool GenerateMipMaps
	);

	CTexture LoadTexture(
		UInt8* Buffer,
		Int32 BufferLength,
		Bool GenerateMipMaps
	);

	CFont* GetFont(
		Int32 FontSize,
		Int32 FontWeight,
		String FontName
	);
	
	Void RenderTexture(
		CUITexture Texture,
		CUIRect Rect
	);

	Void RenderTexture(
		CUISpriteTexture Texture,
		CUIRect TextureRect,
		CUIPoint Anchor,
		CUIPoint Position,
		CUIColor Color
	);

	Void RenderTexture(
		CTexture* Texture,
		CUIRect TextureRect,
		CUIPoint Anchor,
		CUIPoint Position,
		CUIColor Color
	);

	Void RenderTexture(
		CTexture* Texture,
		Int32 RectX,
		Int32 RectY,
		Int32 RectWidth,
		Int32 RectHeight,
		Float32 AnchorX,
		Float32 AnchorY,
		Float32 PositionX,
		Float32 PositionY,
		CUIColor Color
	);

	Void RenderText(
		CFont* Font,
		String Text,
		CUIRect Rect,
		DWORD Format,
		CUIColor Color
	);

	CUIRect GetTextRect(
		CFont* Font,
		String Text,
		CUIRect Rect,
		DWORD Format,
		CUIColor Color
	);
};
