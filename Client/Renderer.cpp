#include "Application.h"
#include "DDSTextureLoader_DirectX9.h"
#include "Renderer.h"
#include "Texture.h"


CFont::CFont(
    Int32 FontSize, 
    Int32 FontWeight, 
    String FontName, 
    LPD3DXFONT Handle
) : FontSize(FontSize), 
    FontWeight(FontWeight), 
    FontName(FontName), 
    Handle(Handle) {
}

CFont::CFont() : 
    FontSize(0),
    FontWeight(0),
    FontName(""),
    Handle(nullptr) {
}

CRenderer::CRenderer(
    HWND Window
) {
    this->Driver = Direct3DCreate9(D3D_SDK_VERSION);
    if (!this->Driver) {
        Fatal("Error creating Direct3D driver!");
    }

    this->Resolution = { 1024, 768 };
    this->ClearColor = { 255, 160, 80, 160 };

    D3DPRESENT_PARAMETERS PresetParameters;
    ZeroMemory(&PresetParameters, sizeof(PresetParameters));
    PresetParameters.Windowed = TRUE;
    PresetParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    PresetParameters.hDeviceWindow = Window;
    PresetParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
    PresetParameters.BackBufferWidth = this->Resolution.Width;
    PresetParameters.BackBufferHeight = this->Resolution.Height;
    PresetParameters.EnableAutoDepthStencil = true;
    PresetParameters.AutoDepthStencilFormat = D3DFMT_D16;

    HRESULT Result = 0;
    Result = this->Driver->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        Window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &PresetParameters,
        &this->Device
    );

    if (Result != D3D_OK) {
        Fatal("Error creating Direct3D device!");
    }

    Result = D3DXCreateSprite(
        this->Device,
        &this->Sprite
    );

    if (Result != D3D_OK) {
        Fatal("Error creating Direct3D sprite!");
    }

    D3DXMatrixIdentity(&this->TransformMatrix);
}

CRenderer::~CRenderer() {
    for (auto Iterator = this->Fonts.begin(); Iterator != this->Fonts.end(); Iterator++) {
        Iterator->second.Handle->Release();
    }

    this->Sprite->Release();
    this->Device->Release();
    this->Driver->Release();
}

CUIRect CRenderer::GetBounds() {
    return CUIRect(0, 0, Resolution.Width, Resolution.Height);
}

CTexture CRenderer::LoadTexture(
    String FilePath,
    Bool GenerateMipMaps
) {
    const size_t FilePathSize = FilePath.size() + 1;
    wchar_t* FilePathW = new wchar_t[FilePathSize];
    mbstowcs(FilePathW, FilePath.c_str(), FilePathSize);

    LPDIRECT3DTEXTURE9 Handle;
    HRESULT Result = DirectX::CreateDDSTextureFromFile(
        this->Device,
        FilePathW,
        &Handle,
        GenerateMipMaps
    );

    delete[] FilePathW;

    if (Result != S_OK) {
        Fatal("Couldn't load texture from file: %s!", FilePath.c_str());
    }

    return CTexture(Handle);
}

CTexture CRenderer::LoadTexture(
    UInt8* Buffer,
    Int32 BufferLength,
    Bool GenerateMipMaps
) {
    LPDIRECT3DTEXTURE9 Handle;
    HRESULT Result = DirectX::CreateDDSTextureFromMemory(
        this->Device,
        Buffer,
        BufferLength,
        &Handle,
        GenerateMipMaps
    );

    if (Result != S_OK) {
        Fatal("Couldn't load texture from memory!");
    }

    return CTexture(Handle);
}

CFont* CRenderer::GetFont(
    Int32 FontSize,
    Int32 FontWeight,
    String FontName
) {
    String Identifier = String(FontName);
    Identifier = Identifier.append(std::to_string(FontSize));
    Identifier = Identifier.append(std::to_string(FontWeight));

    auto Iterator = this->Fonts.find(Identifier);
    if (Iterator == this->Fonts.end()) {
        CFont Font(FontSize, FontWeight, FontName, nullptr);

        HRESULT Result = D3DXCreateFont(
            this->Device,
            FontSize,
            0,
            FontWeight,
            1,
            FALSE,
            DEFAULT_CHARSET,
            OUT_STRING_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            FontName.c_str(), // TODO: Load font from files...
            &Font.Handle
        );

        if (Result != D3D_OK) {
            Fatal("Error creating Direct3D font!");
        }

        this->Fonts[Identifier] = Font;
    }

    return &this->Fonts[Identifier];
}

Void CRenderer::Render() {
    this->Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    this->Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    this->Device->Clear(0, NULL, D3DCLEAR_TARGET, this->ClearColor.Value, 1.0f, 0);
    this->Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    this->Device->BeginScene();

    // TODO: Render here...

    this->Device->EndScene();
    this->Device->Present(NULL, NULL, NULL, NULL);
}

Void CRenderer::BeginSceneUI() {
    this->Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    this->Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    this->Device->Clear(0, NULL, D3DCLEAR_TARGET, this->ClearColor.Value, 1.0f, 0);
    this->Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    this->Device->BeginScene();
}

Void CRenderer::EndSceneUI() {
    this->Device->EndScene();
    this->Device->Present(NULL, NULL, NULL, NULL);
}

Void CRenderer::RenderTexture(
    CUITexture Texture,
    CUIRect Rect
) {
    if (!Texture.Handle) return;

    RECT SourceRect{};
    SourceRect.left = Texture.Frame.Left;
    SourceRect.top = Texture.Frame.Top;
    SourceRect.right = Texture.Frame.Right;
    SourceRect.bottom = Texture.Frame.Bottom;

    D3DXVECTOR3 Translation(Rect.Left, Rect.Top, 0.0f);
    D3DXVECTOR3 Center(0.0f, 0.0f, 0.0f);

    Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    Sprite->SetTransform(&this->TransformMatrix);

    Sprite->Draw(
        (LPDIRECT3DTEXTURE9)Texture.Handle,
        &SourceRect,
        &Center,
        &Translation,
        D3DCOLOR_RGBA(0xFF, 0xFF, 0xFF, 0xFF)
    );

    Sprite->End();
}

Void CRenderer::RenderTexture(
    CUISpriteTexture Texture,
    CUIRect TextureRect,
    CUIPoint Anchor,
    CUIPoint Position,
    CUIColor Color
) {
    RECT SourceRect{};
    SourceRect.left = TextureRect.Left;
    SourceRect.top = TextureRect.Top;
    SourceRect.right = TextureRect.Right;
    SourceRect.bottom = TextureRect.Bottom;

    D3DXVECTOR3 Translation(Position.X, Position.Y, 0.0f);
    D3DXVECTOR3 Center(Anchor.X, Anchor.Y, 0.0f);

    Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    Sprite->SetTransform(&this->TransformMatrix);

    Sprite->Draw(
        (LPDIRECT3DTEXTURE9)Texture,
        &SourceRect,
        &Center,
        &Translation,
        D3DCOLOR(Color.GetValue())
    );

    Sprite->End();
}

Void CRenderer::RenderTexture(
    CTexture* Texture,
    CUIRect TextureRect,
    CUIPoint Anchor,
    CUIPoint Position,
    CUIColor Color
) {
    RECT SourceRect{};
    SourceRect.left = TextureRect.Left;
    SourceRect.top = TextureRect.Top;
    SourceRect.right = TextureRect.Right;
    SourceRect.bottom = TextureRect.Bottom;

    D3DXVECTOR3 Translation(Position.X, Position.Y, 0.0f);
    D3DXVECTOR3 Center(Anchor.X, Anchor.Y, 0.0f);

    Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    Sprite->SetTransform(&this->TransformMatrix);

    Sprite->Draw(
        Texture->Handle,
        &SourceRect,
        &Center,
        &Translation,
        D3DCOLOR(Color.GetValue())
    );

    Sprite->End();
}

Void CRenderer::RenderTexture(
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
) {
    RECT SourceRect{};
    SourceRect.left = RectX;
    SourceRect.top = RectY;
    SourceRect.right = RectX + RectWidth;
    SourceRect.bottom = RectY + RectHeight;

    D3DXVECTOR3 Position(
        static_cast<float>(PositionX), 
        static_cast<float>(PositionY), 
        0.0f
    );

    D3DXVECTOR3 Center(
        static_cast<float>(AnchorX), 
        static_cast<float>(AnchorY),
        0.0f
    );

    Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    Sprite->SetTransform(&this->TransformMatrix);

    Sprite->Draw(
        Texture->Handle,
        &SourceRect,
        &Center,
        &Position,
        D3DCOLOR(Color.GetValue())
    );

    Sprite->End();
}

Void CRenderer::RenderText(
    CFont* Font,
    String Text,
    CUIRect Rect,
    DWORD Format,
    CUIColor Color
) {
    RECT TextRect{};
    TextRect.left = Rect.Left;
    TextRect.top = Rect.Top;
    TextRect.right = Rect.Right;
    TextRect.bottom = Rect.Bottom;

    Sprite->Begin(D3DXSPRITE_ALPHABLEND);
    Sprite->SetTransform(&this->TransformMatrix);
    
    Font->Handle->DrawTextA(
        Sprite,
        Text.c_str(),
        -1,
        &TextRect,
        Format,
        D3DCOLOR_RGBA(Color.R, Color.G, Color.B, Color.A)
    );

    Sprite->End();
}