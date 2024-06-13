#pragma once

#include "Base.h"
#include "Primitives.h"
#include "Texture.h"
#include "UIButton.h"
#include "UIColor.h"
#include "UICheckBox.h"
#include "UIContainer.h"
#include "UIControl.h"
#include "UIEditBox.h"
#include "UIGaugeBar.h"
#include "UIGridBox.h"
#include "UIProgressBar.h"
#include "UIScrollBar.h"
#include "UISliderBar.h"
#include "UISprite.h"
#include "UITabPanel.h"
#include "UITextBox.h"
#include "UITextBoxGroup.h"
#include "UITexture.h"
#include "UITree.h"

struct CUIImage {
    String Name;
    Int32 Width;
    Int32 Height;
    CUITexture Texture;

    CUIImage(
        String Name,
        Int32 Width,
        Int32 Height,
        CUITexture Texture
    );
};

struct CUIEvent {
    Int32 EventID;
    String Target1;
    String Target2;

    CUIEvent(
        Int32 EventID,
        String Target1,
        String Target2
    );
};

struct CUILabel {
    String Name;
    Int32 ID;
    CUIRect Rect;
    Int32 Bold;
    CUIColor ColorNormal;
    CUIColor ColorHover;
    CUIColor ColorPressed;
    UInt32 Alignment;
    Int32 TextIndex;

    CUILabel(
        String Name,
        Int32 ID,
        CUIRect Rect,
        Int32 Bold,
        CUIColor ColorNormal,
        CUIColor ColorHover,
        CUIColor ColorPressed,
        UInt32 Alignment,
        Int32 TextIndex
    );
};

enum CUIElementType : Int32 {
    Container = 0,
    Button,
    EditBox,
    GridBox,
    TextBox,
    ProgressBar,
    Sprite,
    CheckBox,
    SliderBar,
    Tree,
    Tri,
    ScrollBar,
    TabPanel,
    Mtl,
    CobTab,
    Unk,
    GaugeBar,
};

enum CUICursorType : Int32 {
    Move = 0,
    Stop,
    Attack,
    Talk,
    PickUp,
    Event,
    Paint,
    Extract,
    Extend,
    Enhancer,
    Freeze,
    Untrain,
    Rename,
    Change,
    Safety,
    Gem,
    CostumeHanger,
    ArmorBinder,
    UntrainSelective,
    DeleteForceCore,
    EffectorKey,
    Evaluation,
    Honer,
    ItemProtect,
};

struct CUIAlignment {
    union {
        struct {
            UInt32 AxisX : 2;
            UInt32 AxisY : 2;
            UInt32 UnknownX1 : 2;
            UInt32 UnknownY1 : 2;
            UInt32 Unknown1 : 8;
            UInt32 Unknown2 : 8;
            UInt32 Unknown3 : 8;
        };
        UInt32 RawValue;
    };
};

struct CUIElement {
    String Name;
    String ParentName;
    String Key;
    Int32 Unknown0;
    UInt32 ID;
    Int32 ParentID;
    CUIElementType Type;
    CUIRect Rect;
    Float32 AnchorX;
    Float32 AnchorY;
    Int32 Render;
    Int32 Init;
    
    union {
        struct { Int32 Extra1; Int32 Extra2; };
        struct { Int32 ObscureText; Int32 MaxInputLength; };
    };

    Int32 Focus;
    CUIAlignment Alignment;
    CArray<CUIEvent*> Events;
    CArray<CUIImage*> Sprites;
    CArray<CUILabel*> Labels;
    CArray<CUIElement*> Children;
    CUIControlState State;
    Int32 ExtraState1;
    Int32 ExtraState2;
    String InputValue;

    CUIElement(
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
    );
};

using CUIEventHandler = std::function<void(CUIManager*, CUIElement*, CUIEvent*)>;

class CUIManager {
    friend class CApplication;

    CApplication* Application;
    CUIControl* Root;

    CArray<CUIImage> Textures;
    CArray<CUIColor> Colors;
    CArray<CUIImage> Sprites;
    CArray<CUIEvent> Events;
    CArray<CUILabel> Labels;

    CArray<CUIElement> Elements;
    CMap<CUICursorType, CTexture> Cursors;
    CMap<String, Int32> NameToElementIndex;
    CMap<Int32, CUIEventHandler> EventHandlers;
    CUIInputState InputState;
    CUICursorType CursorType;
    CUIElement* FocusElement;

    CUIManager(
        CApplication* Application
    );

    ~CUIManager();

    Void BuildControl(
        CUIControl* ParentControl,
        CUIElement* Element
    );

    Bool LoadData(
        CFileManager* FileManager,
        CRenderer* Renderer
    );

    CUIElement* GetElement(
        String Name
    );

    Bool ElementContains(
        CUIElement* Element,
        Int32 X,
        Int32 Y
    ) {
        return false;
    }

    Void SetElementActive(
        String ElementName,
        Bool Active
    );

    Void ToggleElementActive(
        String ElementName
    );

    template <typename F>
    Void SetEventHandler(
        Int32 EventID,
        F Handler
    );

    Void Update();

    Void UpdateState(
        CUIElement* Element
    );

    Void Update(
        CUIElement* Element
    );

    Bool Focus(
        CUIElement* Element
    );

    Bool TriggerEvent(
        CUIElement* Source,
        CUIEvent* Event
    );

    Void Render(
        CRenderer* Renderer
    );

    Void Render(
        CRenderer* Renderer,
        CUIControl* Control
    );

    Void Render(
        CRenderer* Renderer,
        CUIElement* Parent,
        CUIElement* Element
    );
};
