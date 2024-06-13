#include "Application.h"
#include "FileManager.h"
#include "Localization.h"
#include "Renderer.h"
#include "UIManager.h"

CUIColor::CUIColor(
    UInt8 R,
    UInt8 G,
    UInt8 B,
    UInt8 A
) : R(R), 
    G(G),
    B(B), 
    A(A) {
}

UInt32 CUIColor::GetValue() {
    UInt32 Value = 0;
    Value |= R << 24;
    Value |= G << 16;
    Value |= B << 8;
    Value |= A << 0;
    return Value;
}

CUIImage::CUIImage(
    String Name,
    Int32 Width,
    Int32 Height,
    CUITexture Texture
) : Name(Name),
    Width(Width),
    Height(Height),
    Texture(Texture) {
}

CUIEvent::CUIEvent(
    Int32 EventID,
    String Target1,
    String Target2
) : EventID(EventID),
    Target1(Target1),
    Target2(Target2) {
}

CUILabel::CUILabel(
    String Name,
    Int32 ID,
    CUIRect Rect,
    Int32 Bold,
    CUIColor ColorNormal,
    CUIColor ColorHover,
    CUIColor ColorPressed,
    UInt32 Alignment,
    Int32 TextIndex
) : Name(Name),
    ID(ID),
    Rect(Rect),
    Bold(Bold),
    ColorNormal(ColorNormal),
    ColorHover(ColorHover),
    ColorPressed(ColorPressed),
    Alignment(Alignment),
    TextIndex(TextIndex) {
}

CUIElement::CUIElement(
    String Name,
    String ParentName,
    String Key,
    Int32 Unknown0,
    Int32 ID,
    Int32 ParentID,
    CUIElementType Type,
    CUIRect Rect,
    Int32 Render,
    Int32 Init,
    Int32 Extra1,
    Int32 Extra2,
    Int32 Focus,
    CUIAlignment Alignment,
    CArray<CUIEvent*> Events,
    CArray<CUIImage*> Sprites,
    CArray<CUILabel*> Labels
) : Name(Name),
    ParentName(ParentName),
    Key(Key),
    Unknown0(Unknown0),
    ID(ID),
    ParentID(ParentID),
    Type(Type),
    Rect(Rect),
    Render(Render),
    Init(Init),
    Extra1(Extra1),
    Extra2(Extra2),
    Focus(Focus),
    Alignment(Alignment),
    Events(Events),
    Sprites(Sprites),
    Labels(Labels),
    State(CUIControlState::Normal),
    InputValue("") {
}

CUIInputState::CUIInputState() :
    MouseX(0),
    MouseY(0) {
    memset(KeyStates, 0, sizeof(KeyStates));
}

CUIManager::CUIManager(
    CApplication* Application
) : Application(Application),
    CursorType(CUICursorType::Move),
    FocusElement(nullptr),
    Root(new CUIControl(0, "")) {
    Root->Active = true;
    Root->Rect = CUIRect(0, 0, 1024, 768);
    Root->AlignmentX = CUIAxisAlignment::Stretch;
    Root->AlignmentY = CUIAxisAlignment::Stretch;
}

CUIManager::~CUIManager() {
}

Bool CUIManager::LoadData(
	CFileManager* FileManager,
    CRenderer* Renderer
) {
    auto File = FileManager->LoadFile(this->Application->LanguageDirectory, "ui.dts");
    if (!File) return false;

    StringStream FileStream((CString)File->Data);
    String CurrentLine;
    while (std::getline(FileStream, CurrentLine)) {
        StringStream LineStream(CurrentLine);

        auto DelimiterPosition = CurrentLine.find('@');
        if (DelimiterPosition == std::string::npos) continue;

        auto Key = CurrentLine.substr(0, DelimiterPosition);
        auto Value = CurrentLine.substr(DelimiterPosition + 1);
        this->Application->GetLocalization()->InsertLabel(Key, Value);
    }

	File = FileManager->LoadFile("ui.dat");
	if (!File) return false;

    UInt8* Cursor = (UInt8*)File->Data;
    UInt8* End = Cursor + File->Length;

    if (Cursor + sizeof(UInt16) > End) return false;
    UInt16 Version = *(UInt16*)Cursor;
    Cursor += sizeof(UInt16);

    if (Version < 5) {
        if (Cursor + sizeof(UInt16) * 3 > End) return false;
        Cursor += sizeof(UInt16) * 3;
    }
    else {
        if (Cursor + sizeof(UInt16) * 5 > End) return false;
        Cursor += sizeof(UInt16) * 5;
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 TextureCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    Char Name[MAX_PATH];
    for (UInt32 Index = 0; Index < TextureCount; Index += 1) {
        if (Cursor + sizeof(UInt16) > End) return false;
        UInt16 NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Name, MIN(MAX_PATH, NameLength + 1), (CString)Cursor);
        Cursor += NameLength;

        if (Cursor + sizeof(UInt32) > End) return false;
        Int32 Width = *(UInt32*)Cursor;
        Cursor += sizeof(UInt32);

        if (Cursor + sizeof(UInt32) > End) return false;
        Int32 Height = *(UInt32*)Cursor;
        Cursor += sizeof(UInt32);

        String FilePath = String(this->Application->ResourceDirectory);
        FilePath = FilePath.append("\\");
        FilePath = FilePath.append(Name);
          
        CTexture RenderTexture = Renderer->LoadTexture(FilePath, false);
        CUITexture Texture(RenderTexture.Handle, CUIRect(0, 0, Width, Height));
        CUIImage Image(String(Name), Width, Height, Texture);
        this->Textures.push_back(Image);
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 ColorCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    for (UInt32 Index = 0; Index < ColorCount; Index += 1) {
        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 B = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 G = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 R = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 A = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        CUIColor Color(R, G, B, A);
        this->Colors.push_back(Color);
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 ImageCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    for (UInt32 Index = 0; Index < ImageCount; Index += 1) {
        if (Cursor + sizeof(Int32) > End) return false;
        Int32 TextureIndex = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 X = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Y = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Width = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Height = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        CUIImage* Image = &this->Textures[TextureIndex];
        CUIRect Rect(X, Y, X + Width, Y + Height);
        CUITexture Texture(Image->Texture.Handle, Rect);
        CUIImage Sprite(Image->Name, Image->Width, Image->Height, Texture);
        this->Sprites.push_back(Sprite);
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 EventCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    Char Target[MAX_PATH];
    for (UInt32 Index = 0; Index < EventCount; Index += 1) {
        if (Cursor + sizeof(Int32) > End) return false;
        Int32 EventID = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(UInt16) > End) return false;
        UInt16 NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Target, MAX_PATH, (CString)Cursor);
        Target[NameLength] = '\0';
        Cursor += NameLength;

        String Target1(Target);

        if (Cursor + sizeof(UInt16) > End) return false;
        NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Target, MAX_PATH, (CString)Cursor);
        Target[NameLength] = '\0';
        Cursor += NameLength;

        String Target2(Target);
        CUIEvent Event(EventID, Target1, Target2);
        this->Events.push_back(Event);
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 LabelCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    Char LabelName[MAX_PATH];
    for (UInt32 Index = 0; Index < LabelCount; Index += 1) {
        if (Cursor + sizeof(UInt16) > End) return false;
        UInt16 NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(LabelName, MAX_PATH, (CString)Cursor);
        Cursor += NameLength;

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 ID = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 X = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Y = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Width = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Height = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Bold = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 NormalB = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 NormalG = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 NormalR = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 NormalA = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 HoverB = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 HoverG = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 HoverR = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 HoverA = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 PressedB = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 PressedG = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 PressedR = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt8) > End) return false;
        UInt8 PressedA = *(UInt8*)Cursor;
        Cursor += sizeof(UInt8);

        if (Cursor + sizeof(UInt32) > End) return false;
        UInt32 Alignment = *(UInt32*)Cursor;
        Cursor += sizeof(UInt32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 TextIndex = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        CUIRect Rect(X, Y, X + Width, Y + Height);
        CUIColor NormalColor(NormalR, NormalG, NormalB, NormalA ? NormalA : 0xFF);
        CUIColor HoverColor(HoverR, HoverG, HoverB, HoverA ? HoverA : 0xFF);
        CUIColor PressedColor(PressedR, PressedG, PressedB, PressedA ? PressedA : 0xFF);
        CUILabel Label(
            Name,
            ID,
            Rect,
            Bold,
            NormalColor,
            HoverColor,
            PressedColor,
            Alignment,
            TextIndex
        );
        this->Labels.push_back(Label);
    }

    if (Cursor + sizeof(UInt32) > End) return false;
    UInt32 ElementCount = *(UInt32*)Cursor;
    Cursor += sizeof(UInt32);

    for (UInt32 Index = 0; Index < ElementCount; Index += 1) {
        if (Cursor + sizeof(UInt16) > End) return false;
        UInt16 NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Name, MAX_PATH, (CString)Cursor);
        Name[NameLength] = '\0';
        Cursor += NameLength;

        String ElementName(Name);

        if (Cursor + sizeof(UInt16) > End) return false;
        NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Name, MAX_PATH, (CString)Cursor);
        Name[NameLength] = '\0';
        Cursor += NameLength;

        String ParentName(Name);

        if (Cursor + sizeof(UInt16) > End) return false;
        NameLength = *(UInt16*)Cursor;
        Cursor += sizeof(UInt16);

        if (Cursor + NameLength > End) return false;
        CStringCopySafe(Name, MAX_PATH, (CString)Cursor);
        Name[NameLength] = '\0';
        Cursor += NameLength;

        String Key(Name);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Unknown0 = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 ID = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 ParentID = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Type = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 X = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Y = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Width = *(Int32*)Cursor - X;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Height = *(Int32*)Cursor - Y;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Render = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Init = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Extra1 = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Extra2 = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 Focus = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 EventIndex = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 EventCount = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 SpriteIndex = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 SpriteCount = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 LabelIndex = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(Int32) > End) return false;
        Int32 LabelCount = *(Int32*)Cursor;
        Cursor += sizeof(Int32);

        if (Cursor + sizeof(UInt32) > End) return false;
        CUIAlignment Alignment;
        Alignment.RawValue = *(UInt32*)Cursor;
        Cursor += sizeof(UInt32);

        CArray<CUIEvent*> Events;
        for (Int32 Offset = 0; Offset < EventCount; Offset += 1) {
            Events.push_back(&this->Events[EventIndex + Offset]);
        }

        CArray<CUIImage*> Sprites;
        for (Int32 Offset = 0; Offset < SpriteCount; Offset += 1) {
            Sprites.push_back(&this->Sprites[SpriteIndex + Offset]);
        }

        CArray<CUILabel*> Labels;
        for (Int32 Offset = 0; Offset < LabelCount; Offset += 1) {
            Labels.push_back(&this->Labels[LabelIndex + Offset]);
        }

        CUIRect Rect(X, Y, X + Width, Y + Height);
        CUIElement Element(
            ElementName,
            ParentName,
            Key,
            Unknown0,
            ID,
            ParentID,
            (CUIElementType)Type,
            Rect,
            Render,
            Init,
            Extra1,
            Extra2,
            Focus,
            Alignment,
            Events,
            Sprites,
            Labels
        );
        Element.ExtraState1 = Element.Extra1;
        Element.ExtraState2 = Element.Extra2;
        this->Elements.push_back(Element);
        this->NameToElementIndex[ElementName] = this->Elements.size() - 1;
    }

    for (auto ElementIndex = 0; ElementIndex < this->Elements.size(); ElementIndex += 1) {
        auto Element = &this->Elements[ElementIndex];
        auto Parent = this->GetElement(Element->ParentName);
        if (!Parent) continue;

        Parent->Children.push_back(Element);
    }

    for (auto ElementIndex = 0; ElementIndex < this->Elements.size(); ElementIndex += 1) {
        auto Element = &this->Elements[ElementIndex];
        if (!Element->ParentName.empty()) continue;
        BuildControl(Root, Element);
    }

    struct { CUICursorType Type; CString FileName; } Cursors[] = {
        { CUICursorType::Move, "Cursor_Move.dds" },
        { CUICursorType::Stop, "Cursor_Stop.dds" },
        { CUICursorType::Attack, "Cursor_Attack.dds" },
        { CUICursorType::Talk, "Cursor_Talk.dds" },
        { CUICursorType::PickUp, "Cursor_PickUp.dds" },
        { CUICursorType::Event, "Cursor_Event.dds" },
        { CUICursorType::Paint, "Cursor_Paint.dds" },
        { CUICursorType::Extract, "Cursor_Extract.dds" },
        { CUICursorType::Extend, "Cursor_Extend.dds" },
        { CUICursorType::Enhancer, "Cursor_Enhancer.dds" },
        { CUICursorType::Freeze, "Cursor_Freeze.dds" },
        { CUICursorType::Untrain, "Cursor_Untrain.dds" },
        { CUICursorType::Rename, "Cursor_Rename.dds" },
        { CUICursorType::Change, "Cursor_change.dds" },
        { CUICursorType::Safety, "Cursor_Safety.dds" },
        { CUICursorType::Gem, "Cursor_Gem.dds" },
        { CUICursorType::CostumeHanger, "Cursor_CostumeHanger.dds" },
        { CUICursorType::ArmorBinder, "Cursor_ArmorBinder.dds" },
        { CUICursorType::UntrainSelective, "Cursor_Untrain_s.dds" },
        { CUICursorType::DeleteForceCore, "Cursor_delforcecore.dds" },
        { CUICursorType::EffectorKey, "Cursor_effecterkey.dds" },
        { CUICursorType::Evaluation, "Cursor_evaluation.dds" },
        { CUICursorType::Honer, "Cursor_Honer.dds" },
        { CUICursorType::ItemProtect, "Cursor_ItemProtect.dds" },
    };
    Int32 CursorCount = sizeof(Cursors) / sizeof(Cursors[0]);

    for (auto CursorIndex = 0; CursorIndex < CursorCount; CursorIndex += 1) {
        String FilePath = String(this->Application->ResourceDirectory);
        FilePath = FilePath.append("\\UI\\");
        FilePath = FilePath.append(Cursors[CursorIndex].FileName);

        this->Cursors[Cursors[CursorIndex].Type] = Renderer->LoadTexture(FilePath, false);
    }

    this->SetEventHandler(0, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        Manager->SetElementActive(Event->Target1, true);
    });

    this->SetEventHandler(1, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        Manager->SetElementActive(Event->Target1, false);
    });

    this->SetEventHandler(2, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        // TODO: Login event
        //Manager->SetElementActive(Event->Target1, false);
    });

    this->SetEventHandler(3, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        Manager->Application->Exit();
    });

    this->SetEventHandler(7, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        // TODO: Goto previous screen
    });

    this->SetEventHandler(6, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        Manager->SetElementActive(Source->ParentName, false);
        Manager->SetElementActive(Event->Target1, true);
    });

    this->SetEventHandler(23, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        Manager->ToggleElementActive(Event->Target1);
    });

    this->SetEventHandler(52, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        auto Element = Manager->GetElement(Event->Target1);
        if (!Element) return;

        if (!Event->Target2.empty()) {
            for (auto Iterator = Element->Children.begin(); Iterator != Element->Children.end(); ++Iterator) {
                auto Child = *Iterator;
                if (Child->Name == Event->Target2) {
                    Element = Child;
                    break;
                }
            }
        }

        Element->State = (Element->State == CUIControlState::Normal) ? CUIControlState::Active : CUIControlState::Normal;
    });

    this->SetEventHandler(114, [](CUIManager* Manager, CUIElement* Source, CUIEvent* Event) {
        auto Element = Manager->GetElement(Event->Target1);
        if (!Element) return;

        Element->ExtraState1 = std::stoi(Event->Target2);
    });

    SetElementActive(String("A_dlg_login"), true);

    //SetElementActive(String("A_dlg_quickslot_main"), true);
    //SetElementActive(String("A_dlg_mainT"), true);

    return true;
}

Void CUIManager::BuildControl(
    CUIControl* ParentControl,
    CUIElement* Element
) {
    if (Element->Init == -1) return;

    CUIElement* Parent = GetElement(Element->ParentName);
    CUIControl* Control = nullptr;

    if (Element->Type == CUIElementType::Container) {
        //assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() < 2);
        assert(Element->Labels.size() == 0);

        auto Container = new CUIContainer(Element->ID, Element->Name);
        Control = Container;

        if (Element->Sprites.size() > 0) {
            Container->Background = Element->Sprites[0]->Texture;
        }
    }

    if (Element->Type == CUIElementType::Button) {
        auto Button = new CUIButton(Element->ID, Element->Name);
        Control = Button;

        assert(Element->Sprites.size() < 4);
        for (auto Index = 0; Index < Element->Sprites.size(); Index++) {
            Button->Textures[(CUIControlState)Index] = Element->Sprites[Index]->Texture;
        }

        for (auto Label : Element->Labels) {
            auto Key = String(std::to_string(Label->TextIndex));
            auto Text = this->Application->GetLocalization()->LocalizeLabel(Key);
            auto TextBox = new CUITextBox(Label->ID, Label->Name);
            TextBox->Rect = Label->Rect;
            //TextBox->AlignmentX = (CUIAxisAlignment)Label->Alignment.AxisX;
            //TextBox->AlignmentY = (CUIAxisAlignment)Label->Alignment.AxisY;
            TextBox->Text = Text;
            TextBox->Bold = Label->Bold;
            TextBox->Format = Label->Alignment;
            TextBox->Colors[CUIControlState::Normal] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::Active] = Label->ColorPressed;
            TextBox->Colors[CUIControlState::Hover] = Label->ColorHover;
            TextBox->Colors[CUIControlState::Disabled] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::ActiveDisabled] = Label->ColorPressed;

            Control->AddChild(TextBox);
        }
    }

    if (Element->Type == CUIElementType::EditBox) {
        // assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() == 4); // TODO: Find out what the textures are used for
        assert(Element->Labels.size() == 0);
        
        auto EditBox = new CUIEditBox(Element->ID, Element->Name);
        Control = EditBox;
        
        EditBox->ObscureText = Element->ObscureText;
        EditBox->MaxInputLength = Element->MaxInputLength;
    }

    if (Element->Type == CUIElementType::GridBox) {
        assert(Element->Events.size() == 0);
        assert(Element->Labels.size() == 0);

        auto GridBox = new CUIGridBox(Element->ID, Element->Name);
        Control = GridBox;

        for (auto Image : Element->Sprites) {
            GridBox->Textures.push_back(Image->Texture);
        }
    }

    if (Element->Type == CUIElementType::TextBox) {
        assert(Element->Events.size() == 0);

        auto TextBoxGroup = new CUITextBoxGroup(Element->ID, Element->Name);
        Control = TextBoxGroup;

        for (auto Image : Element->Sprites) {
            TextBoxGroup->Textures.push_back(Image->Texture);
        }

        for (auto Label : Element->Labels) {
            auto Key = String(std::to_string(Label->TextIndex));
            auto Text = this->Application->GetLocalization()->LocalizeLabel(Key);
            auto TextBox = new CUITextBox(Label->ID, Label->Name);
            TextBox->Rect = Label->Rect;
            //TextBox->AlignmentX = (CUIAxisAlignment)Label->Alignment.AxisX;
            //TextBox->AlignmentY = (CUIAxisAlignment)Label->Alignment.AxisY;
            TextBox->Text = Text;
            TextBox->Bold = Label->Bold;
            TextBox->Format = Label->Alignment;
            TextBox->Colors[CUIControlState::Normal] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::Active] = Label->ColorPressed;
            TextBox->Colors[CUIControlState::Hover] = Label->ColorHover;
            TextBox->Colors[CUIControlState::Disabled] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::ActiveDisabled] = Label->ColorPressed;

            Control->AddChild(TextBox);
        }
    }

    if (Element->Type == CUIElementType::ProgressBar) {
        assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() == 1);
        assert(Element->Labels.size() == 0);

        auto ProgressBar = new CUIProgressBar(Element->ID, Element->Name);
        Control = ProgressBar;

        ProgressBar->Bar = Element->Sprites[0]->Texture;
    }

    if (Element->Type == CUIElementType::Sprite) {
        // assert(Element->Events.size() == 0);
        assert(Element->Labels.size() == 0);
        auto Sprite = new CUISprite(Element->ID, Element->Name);
        Control = Sprite;

        for (auto Image : Element->Sprites) {
            Sprite->Textures.push_back(Image->Texture);
        }
    }

    if (Element->Type == CUIElementType::CheckBox) {
        auto CheckBox = new CUICheckBox(Element->ID, Element->Name);
        Control = CheckBox;

        assert(Element->Sprites.size() < 6);
        for (auto Index = 0; Index < Element->Sprites.size(); Index++) {
            CheckBox->Textures[(CUIControlState)Index] = Element->Sprites[Index]->Texture;
        }

        for (auto Label : Element->Labels) {
            auto Key = String(std::to_string(Label->TextIndex));
            auto Text = this->Application->GetLocalization()->LocalizeLabel(Key);
            auto TextBox = new CUITextBox(Label->ID, Label->Name);
            TextBox->Rect = Label->Rect;
            //TextBox->AlignmentX = (CUIAxisAlignment)Label->Alignment.AxisX;
            //TextBox->AlignmentY = (CUIAxisAlignment)Label->Alignment.AxisY;
            TextBox->Text = Text;
            TextBox->Bold = Label->Bold;
            TextBox->Format = Label->Alignment;
            TextBox->Colors[CUIControlState::Normal] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::Active] = Label->ColorPressed;
            TextBox->Colors[CUIControlState::Hover] = Label->ColorHover;
            TextBox->Colors[CUIControlState::Disabled] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::ActiveDisabled] = Label->ColorPressed;

            Control->AddChild(TextBox);
        }
    }

    if (Element->Type == CUIElementType::SliderBar) {
        // assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() == 7);
        assert(Element->Labels.size() == 0);
    //    Element->Rect

        auto SliderBar = new CUISliderBar(Element->ID, Element->Name);
        Control = SliderBar;

        SliderBar->Bars[CUIControlState::Normal] = Element->Sprites[0]->Texture;
        SliderBar->Bars[CUIControlState::Active] = Element->Sprites[1]->Texture;
        SliderBar->Bars[CUIControlState::Hover] = Element->Sprites[1]->Texture;
        SliderBar->Bars[CUIControlState::Disabled] = Element->Sprites[0]->Texture;
        SliderBar->Bars[CUIControlState::ActiveDisabled] = Element->Sprites[0]->Texture;

        SliderBar->Knobs[CUIControlState::Normal] = Element->Sprites[2]->Texture;
        SliderBar->Knobs[CUIControlState::Active] = Element->Sprites[3]->Texture;
        SliderBar->Knobs[CUIControlState::Hover] = Element->Sprites[4]->Texture;
        SliderBar->Knobs[CUIControlState::Disabled] = Element->Sprites[3]->Texture;
        SliderBar->Knobs[CUIControlState::ActiveDisabled] = Element->Sprites[3]->Texture;
    }

    if (Element->Type == CUIElementType::Tree) {
        assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() == 0);
        //assert(Element->Labels.size() == 0);

        auto Tree = new CUITree(Element->ID, Element->Name);
        Control = Tree;

        for (auto Label : Element->Labels) {
            auto Key = String(std::to_string(Label->TextIndex));
            auto Text = this->Application->GetLocalization()->LocalizeLabel(Key);
            auto TextBox = new CUITextBox(Label->ID, Label->Name);
            TextBox->Rect = Label->Rect;
            //TextBox->AlignmentX = (CUIAxisAlignment)Label->Alignment.AxisX;
            //TextBox->AlignmentY = (CUIAxisAlignment)Label->Alignment.AxisY;
            TextBox->Text = Text;
            TextBox->Bold = Label->Bold;
            TextBox->Format = Label->Alignment;
            TextBox->Colors[CUIControlState::Normal] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::Active] = Label->ColorPressed;
            TextBox->Colors[CUIControlState::Hover] = Label->ColorHover;
            TextBox->Colors[CUIControlState::Disabled] = Label->ColorNormal;
            TextBox->Colors[CUIControlState::ActiveDisabled] = Label->ColorPressed;

            Control->AddChild(TextBox);
        }
    }

    if (Element->Type == CUIElementType::Tri) {
        auto Tri = new CUIControl(Element->ID, Element->Name);
        Control = Tri;
    }

    if (Element->Type == CUIElementType::ScrollBar) {
        //assert(Element->Events.size() == 0);
        assert(Element->Sprites.size() == 14);
        assert(Element->Labels.size() == 0);

        auto ScrollBar = new CUIScrollBar(Element->ID, Element->Name);
        Control = ScrollBar;

        /*
        <ui_texture id="ui_texture0" x="471" y="32" w="9" h="9" /> arrow button up normal 
        <ui_texture id="ui_texture0" x="481" y="32" w="9" h="9" /> arrow button up active
        <ui_texture id="ui_texture0" x="491" y="32" w="9" h="9" /> arrow button up hover
        <ui_texture id="ui_texture0" x="481" y="32" w="9" h="9" /> arrow button up disabled 
        <ui_texture id="ui_texture0" x="471" y="82" w="9" h="9" /> arrow button down normal 
        <ui_texture id="ui_texture0" x="481" y="82" w="9" h="9" /> arrow button down active
        <ui_texture id="ui_texture0" x="491" y="82" w="9" h="9" /> arrow button down hover
        <ui_texture id="ui_texture0" x="481" y="82" w="9" h="9" /> arrow button down disabled 
        <ui_texture id="ui_texture0" x="471" y="42" w="9" h="39" /> bar normal 
        <ui_texture id="ui_texture0" x="481" y="42" w="9" h="39" /> bar active
        <ui_texture id="ui_texture0" x="491" y="42" w="9" h="39" /> bar hover 
        <ui_texture id="ui_texture0" x="461" y="32" w="9" h="20" /> bar back slice top
        <ui_texture id="ui_texture0" x="461" y="52" w="9" h="20" /> bar back slice center
        <ui_texture id="ui_texture0" x="461" y="71" w="9" h="20" /> bar back slice bottom
        */
    }

    if (Element->Type == CUIElementType::TabPanel) {
        auto TabPanel = new CUITabPanel(Element->ID, Element->Name);
        Control = TabPanel;
    }

    if (Element->Type == CUIElementType::Mtl) {
        auto Mtl = new CUIControl(Element->ID, Element->Name);
        Control = Mtl;
    }

    if (Element->Type == CUIElementType::CobTab) {
        auto CobTab = new CUIControl(Element->ID, Element->Name);
        Control = CobTab;
    }

    if (Element->Type == CUIElementType::Unk) {
        auto Unk = new CUIControl(Element->ID, Element->Name);
        Control = Unk;
    }

    if (Element->Type == CUIElementType::GaugeBar) {
        auto GaugeBar = new CUIGaugeBar(Element->ID, Element->Name);
        Control = GaugeBar;
    }

    Control->Rect = Element->Rect;
//    Control->Position = CUIPoint(Element->Rect.Left, Element->Rect.Top);
//    if (Parent) Control->Position -= CUIPoint(Parent->Rect.Left, Parent->Rect.Top);
//    Control->Size = CUIPoint(Element->Rect.Right - Element->Rect.Left, Element->Rect.Bottom - Element->Rect.Top);
    Control->AlignmentX = (CUIAxisAlignment)Element->Alignment.AxisX;
    Control->AlignmentY = (CUIAxisAlignment)Element->Alignment.AxisY;

    if (ParentControl) ParentControl->AddChild(Control);

    for (auto Child : Element->Children) {
        BuildControl(Control, Child);
    }
} 

CUIElement* CUIManager::GetElement(
    String Name
) {
    auto Iterator = this->NameToElementIndex.find(Name);
    if (Iterator != this->NameToElementIndex.end()) {
        auto Index = Iterator->second;
        return &this->Elements[Index];
    }

    return nullptr;
}

Void CUIManager::SetElementActive(
    String ElementName,
    Bool Active
) {
    auto Control = Root->FindChild(ElementName);
    if (Control) Control->SetActive(Active);
}

Void CUIManager::ToggleElementActive(
    String ElementName
) {
    auto Control = Root->FindChild(ElementName);
    if (Control) Control->SetActive(!Control->Active);
}

template <typename F>
Void CUIManager::SetEventHandler(
    Int32 EventID,
    F Handler
) {
    this->EventHandlers[EventID] = Handler;
}

Void CUIManager::Update() {
    Root->Update(&InputState);

    if (this->InputState.KeyStates['N']) {
        this->InputState.KeyStates['N'] = false;
        /*
        if (!this->ActiveElementIndices.empty()) {
            Int32 PreviousElementIndex = *this->ActiveElementIndices.begin();
            this->ActiveElementIndices.clear();

            for (auto ElementIndex = PreviousElementIndex + 1; ElementIndex < this->Elements.size(); ElementIndex += 1) {
                auto Element = this->Elements[ElementIndex];
                //                if (Element.Init < 0) continue;
                if (Element.ParentName.empty()) {
                    this->SetElementActive(Element.Name, true);
                    break;
                }
            }
        }
        else {
            this->SetElementActive(this->Elements[0].Name, true);
        }
        */
    }

    //Root->Layout(CUIRect(0, 0, 1024, 768));
    return;
    /*
    for (auto Iterator = this->ActiveElementIndices.begin(); Iterator != this->ActiveElementIndices.end(); Iterator++) {
        auto Element = &this->Elements[*Iterator];
        this->UpdateState(Element);
    }

    if (this->InputState.KeyStates[VK_LBUTTON]) {
        for (auto Iterator = this->ActiveElementIndices.begin(); Iterator != this->ActiveElementIndices.end(); Iterator++) {
            auto Element = &this->Elements[*Iterator];
            this->Focus(Element);
        }
    }

    if (this->FocusElement) {
        this->Update(this->FocusElement);
    }

    for (auto Iterator = this->RemovedElementIndices.begin(); Iterator != this->RemovedElementIndices.end(); Iterator++) {
        this->ActiveElementIndices.erase(*Iterator);
    }

    this->RemovedElementIndices.clear();
    */
}

Void CUIManager::UpdateState(
    CUIElement* Element
) {
    Size Resolution = { 1920, 1080 };


    /*
    if (Element->Alignment.AxisY == 1) { // Center
        Element->Rect.Y = (Resolution.Height - Element->Rect.Height) / 2;
    }

    if (Element->Alignment.AxisY == 2) { // Bottom
        Element->Rect.Y = (Resolution.Height - Element->Rect.Height);
    }

    if (Element->Alignment.AxisX == 1) { // Center
        Element->Rect.X = (Resolution.Width - Element->Rect.Width) / 2;
    }

    if (Element->Alignment.AxisX == 2) { // Right
        Element->Rect.X = (Resolution.Width - Element->Rect.Width);
    }
    */

    if (Element->Alignment.AxisY == 0) { // Bottom
        Element->AnchorY = 0.0f;
    }

    if (Element->Alignment.AxisY == 1) { // Center
        Element->AnchorY = 0.5f;
    }

    if (Element->Alignment.AxisY == 2) { // Bottom
        Element->AnchorY = 1.0f;
    }

    if (Element->Alignment.AxisX == 0) { // Bottom
        Element->AnchorX = 0.0f;
    }

    if (Element->Alignment.AxisX == 1) { // Center
        Element->AnchorX = 0.5f;
    }

    if (Element->Alignment.AxisX == 2) { // Bottom
        Element->AnchorX = 1.0f;
    }

    if (Element->Type == CUIElementType::Button) { // TODO: Find out which types do have this also...
        if (this->ElementContains(Element, this->InputState.MouseX, this->InputState.MouseY)) {
            Element->State = this->InputState.KeyStates[VK_LBUTTON] ? CUIControlState::Active : CUIControlState::Hover;
        }
        else {
            Element->State = CUIControlState::Normal;
        }
    }

    if (this->InputState.KeyStates[VK_LBUTTON] &&
        !Element->Events.empty() &&
        this->ElementContains(Element, this->InputState.MouseX, this->InputState.MouseY)) {
        this->InputState.KeyStates[VK_LBUTTON] = false;

        for (auto Iterator = Element->Events.begin(); Iterator != Element->Events.end(); ++Iterator) {
            this->TriggerEvent(Element, *Iterator);
        }
    }

    for (auto Iterator = Element->Children.begin(); Iterator != Element->Children.end(); Iterator++) {
        this->UpdateState(*Iterator);
    }
}

Void CUIManager::Update(
    CUIElement* Element
) {
    return;

    if (this->FocusElement != Element) return;

    if (Element->Type == CUIElementType::EditBox) {
        if (this->InputState.KeyStates[VK_BACK] && !Element->InputValue.empty()) {
            Element->InputValue.pop_back();
            this->InputState.KeyStates[VK_BACK] = false;
        }

        Bool IsFilled = (Element->MaxInputLength > 0 && Element->InputValue.length() >= Element->MaxInputLength);
        if (!IsFilled) {
            for (auto Key = '0'; Key <= '9'; Key += 1) {
                if (this->InputState.KeyStates[Key]) {
                    Element->InputValue.push_back(Key);
                    this->InputState.KeyStates[Key] = false;
                }
            }

            for (auto Key = 'A'; Key <= 'Z'; Key += 1) {
                if (this->InputState.KeyStates[Key]) {
                    Bool Capitalized = (
                        this->InputState.KeyStates[VK_SHIFT] ||
                        this->InputState.KeyStates[VK_LSHIFT] ||
                        this->InputState.KeyStates[VK_RSHIFT]
                    );
                    // bool capsLockActive = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;

                    Element->InputValue.push_back(Capitalized ? toupper(Key) : tolower(Key));
                    this->InputState.KeyStates[Key] = false;
                }
            }
        }

        if (this->InputState.KeyStates[VK_RETURN]) {
            // TODO: Focus and trigger submit button somehow?
        }
    }
}

Bool CUIManager::Focus(
    CUIElement* Element
) {
    //if (!Element->Focus) return false;

    if (this->ElementContains(Element, this->InputState.MouseX, this->InputState.MouseY)) {
        for (auto Iterator = Element->Children.rbegin(); Iterator != Element->Children.rend(); ++Iterator) {
            if (this->Focus(*Iterator)) return true;
        }

        Bool IsFocusable = (
            Element->Type == CUIElementType::EditBox ||
            Element->Type == CUIElementType::CheckBox ||
            Element->Type == CUIElementType::ScrollBar ||
            Element->Type == CUIElementType::TabPanel
        );

        if (IsFocusable) {
            this->FocusElement = Element;
            return true;
        }
    }

    return false;
}

Bool CUIManager::TriggerEvent(
    CUIElement* Source,
    CUIEvent* Event
) {
    Trace("TriggerEvent(%d, %s, %s)", Event->EventID, Event->Target1.c_str(), Event->Target2.c_str());

    auto Iterator = this->EventHandlers.find(Event->EventID);
    if (Iterator != this->EventHandlers.end()) {
        Iterator->second(this, Source, Event);
        return true;
    }

    return false;
}

Void CUIManager::Render(
    CRenderer* Renderer,
    CUIControl* Control
) {
    if (!Control->Active) return;

    D3DXMATRIX ScalingMatrix, TranslationMatrix, TransformMatrix;
    D3DXMatrixScaling(&ScalingMatrix, 1.0f, 1.0f, 1.0f);
    D3DXMatrixIdentity(&ScalingMatrix);
    D3DXMatrixTranslation(&TranslationMatrix, Control->Rect.Left, Control->Rect.Top, 0.0f);
    TransformMatrix = Renderer->TransformMatrix; 
    Renderer->TransformMatrix = ScalingMatrix * TranslationMatrix;

    if (Control->IsVisible) Control->Render(Renderer);

    for (auto& Child : Control->Children) {
        Render(Renderer, Child);
    }

    Renderer->TransformMatrix = TransformMatrix;
}

Void CUIManager::Render(
    CRenderer* Renderer
) {
    /*

    Root->Render(Renderer);
    */
    CUIRect Viewport = Application->GetViewport();
    CUIRect Bounds = Renderer->GetBounds();
    Size Resolution = { 1024, 768 };
    Int32 ScaleX = 1.0f * (Bounds.Right - Bounds.Left) / Resolution.Width;
    Int32 ScaleY = 1.0f * (Bounds.Bottom - Bounds.Top) / Resolution.Height;

    D3DXMATRIX ScalingMatrix, TranslationMatrix, TransformMatrix;
    D3DXMatrixScaling(&ScalingMatrix, ScaleX, ScaleY, 1.0f);
    D3DXMatrixTranslation(&TranslationMatrix, 0.0f, 0.0f, 0.0f);
    Render(Renderer, Root);

    Renderer->RenderTexture(
        &this->Cursors[this->CursorType],
        0,
        0,
        32,
        32,
        0,
        0,
        this->InputState.MouseX,
        this->InputState.MouseY,
        CUIColor(255, 255, 255, 255)
    );

    return;
    /*
    for (auto Iterator = this->ActiveElementIndices.begin(); Iterator != this->ActiveElementIndices.end(); Iterator++) {
        auto Element = &this->Elements[*Iterator];
        this->Render(Renderer, nullptr, Element);
    }

    Renderer->RenderTexture(
        &this->Cursors[this->CursorType],
        0,
        0,
        32,
        32,
        0,
        0,
        this->InputState.MouseX,
        this->InputState.MouseY,
        CUIColor(255, 255, 255, 255)
    );
    */
}

Void CUIManager::Render(
    CRenderer* Renderer,
    CUIElement* Parent,
    CUIElement* Element
) {
    if (Element->Render < 0) return;

    if (Element->Type == CUIElementType::TabPanel && Parent) {
        if (Element->Extra1 != Parent->ExtraState1) return;
    }
    /*
    Int32 PositionX = (Parent) ? Element->RelativeX + Parent->Rect.X : Element->Rect.X;
    Int32 PositionY = (Parent) ? Element->RelativeY + Parent->Rect.Y : Element->Rect.Y;
    */

    Int32 PositionX = Element->Rect.Left;
    Int32 PositionY = Element->Rect.Top;
    /*
    if (Element->Type == CUIElementType::Button ||
        Element->Type == CUIElementType::CheckBox) {
        if (Element->Sprites.size() > Element->State) {
            auto Sprite = Element->Sprites[Element->State];

            Renderer->RenderTexture(
                &Sprite->Texture->RenderTexture,
                Sprite->Rect.Left,
                Sprite->Rect.Top,
                Sprite->Rect.Right - Sprite->Rect.Left,
                Sprite->Rect.Bottom - Sprite->Rect.Top,
                Element->AnchorX,
                Element->AnchorY,
                PositionX,
                PositionY,
                CUIColor(255, 255, 255, 255)
            );
        }
    }
    */
    /*
    for (auto Iterator = Element->Sprites.begin(); Iterator != Element->Sprites.end(); Iterator++) {
        auto Sprite = *Iterator;

        Renderer->RenderTexture(
            &Sprite->Texture->RenderTexture,
            Sprite->Rect.X,
            Sprite->Rect.Y,
            Sprite->Rect.Width,
            Sprite->Rect.Height,
            0,
            0,
            Element->Rect.X,
            Element->Rect.Y,
            CUIColor(255, 255, 255, 255)
        );
    }
    */
    for (auto Iterator = Element->Labels.begin(); Iterator != Element->Labels.end(); Iterator++) {
        auto Label = *Iterator;
        auto Key = String(std::to_string(Label->TextIndex));
        auto Text = this->Application->GetLocalization()->LocalizeLabel(Key);

        Int32 FontSize = 12;
        /*
        if (Element->Type == CUIElementType::TextBox) {
            FontSize = Label->Rect.Height;
        }
        */
        auto Font = Renderer->GetFont(FontSize, Label->Bold ? FW_NORMAL : FW_BOLD, "Arial");

        Renderer->RenderText(
            Font,
            Text,
            Label->Rect,
            Label->Alignment,
            CUIColor(255, 255, 255, 255)
        );
    }
    /*
    if (Element->Type == CUIElementType::Container ||
        Element->Type == CUIElementType::Sprite) {
        for (auto Iterator = Element->Sprites.begin(); Iterator != Element->Sprites.end(); Iterator++) {
            auto Sprite = *Iterator;

            Renderer->RenderTexture(
                &Sprite->Texture->RenderTexture,
                Sprite->Rect.Left,
                Sprite->Rect.Top,
                Sprite->Rect.Right - Sprite->Rect.Left,
                Sprite->Rect.Bottom - Sprite->Rect.Top,
                Element->AnchorX,
                Element->AnchorY,
                PositionX,
                PositionY,
                CUIColor(255, 255, 255, 255)
            );
        }
    }
    */
    for (auto Iterator = Element->Children.begin(); Iterator != Element->Children.end(); Iterator++) {
        this->Render(Renderer, Element, *Iterator);
    }

    if (Element->Type == CUIElementType::EditBox) {
        if (!Element->InputValue.empty()) {
            String Text = Element->InputValue;

            if (Element->ObscureText) {
                Text = String(Text.length(), '*');
            }

            auto Font = Renderer->GetFont(Element->Rect.Bottom - Element->Rect.Top, FW_NORMAL, "Arial");
            Renderer->RenderText(
                Font,
                Text,
                Element->Rect,
                Element->Alignment.RawValue,
                CUIColor(255, 255, 255, 255)
            );
        }
    }
}