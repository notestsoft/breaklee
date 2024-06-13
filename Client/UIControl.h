#pragma once

#include "Base.h"
#include "UIColor.h"
#include "UIRect.h"

enum CUIControlState : Int32 {
    Normal = 0,
    Active = 1,
    Hover = 2,
    Disabled = 3,
    ActiveDisabled = 4,
};

enum CUIAxisAlignment : Int32 {
    Leading,
    Center,
    Trailing,
    Stretch,
    Resize
};

struct CUIInputState {
    Int32 MouseX;
    Int32 MouseY;
    Bool KeyStates[0xFF];

    CUIInputState();
};

class CUIControl {
    friend class CUIManager;

protected:
    UInt32 ID;
    String Name;
    Bool Active;
    CUIControlState State;
    CUIControl* Parent;
    CArray<CUIControl*> Children;

public:
    CUIRect Rect;
    CUIAxisAlignment AlignmentX;
    CUIAxisAlignment AlignmentY;
    Bool IsVisible;

    CUIControl(UInt32 ID, String Name) :
        ID(ID),
        Name(Name),
        Rect(CUIRect()),
        AlignmentX(CUIAxisAlignment::Leading),
        AlignmentY(CUIAxisAlignment::Leading),
        IsVisible(true),
        Active(false),
        State(CUIControlState::Normal),
        Parent(nullptr),
        Children({}) {
    }

    Void SetActive(Bool NewValue) {
        Active = NewValue;
        for (auto& Child : Children) Child->SetActive(NewValue);
    }

    Void AddChild(CUIControl* Child) {
        assert(!Child->Parent);
        Child->Parent = this;
        Children.push_back(Child);
    }

    CUIControl* FindChild(UInt32 ChildID) {
        for (auto Child : Children) {
            if (Child->ID == ChildID) return Child;
        }

        for (auto Child : Children) {
            auto Control = Child->FindChild(ChildID);
            if (Control) return Control;
        }

        return nullptr;
    }

    CUIControl* FindChild(String ChildName) {
        for (auto Child : Children) {
            if (Child->Name == ChildName) return Child;
        }

        for (auto Child : Children) {
            auto Control = Child->FindChild(ChildName);
            if (Control) return Control;
        }

        return nullptr;
    }

    Void RemoveChild(CUIControl* Child) {
        for (auto Iterator = Children.begin(); Iterator != Children.end(); Iterator++) {
            if (*Iterator != Child) continue;

            (*Iterator)->Parent = nullptr;
            Children.erase(Iterator);
        }
    }

    Void RemoveAllChildren() {
        for (auto Child : Children) {
            Child->Parent = nullptr;
        }

        Children.clear();
    }

    Bool Contains(CUIPoint Point) {
        return false;
//        return Rect.Contains(Point);
    }

    CUIRect CalculateLayout(CUIRect Bounds, CUIRect SourceRect) {
        CUIRect ResultRect;

        switch (AlignmentX) {
        case CUIAxisAlignment::Leading:
            ResultRect.Left = Bounds.Left + SourceRect.Left;
            ResultRect.Right = Bounds.Left + SourceRect.Right;
            break;

        case CUIAxisAlignment::Center:
            ResultRect.Left = Bounds.Left + (Bounds.Right - Bounds.Left - (SourceRect.Right - SourceRect.Left)) / 2 + SourceRect.Left;
            ResultRect.Right = ResultRect.Left + (SourceRect.Right - SourceRect.Left);
            break;

        case CUIAxisAlignment::Trailing:
            ResultRect.Left = Bounds.Right - (SourceRect.Right - SourceRect.Left) - SourceRect.Left;
            ResultRect.Right = ResultRect.Left + (SourceRect.Right - SourceRect.Left);
            break;
        }

        switch (AlignmentY) {
        case CUIAxisAlignment::Leading:
            ResultRect.Top = Bounds.Top + SourceRect.Top;
            ResultRect.Bottom = Bounds.Top + SourceRect.Bottom;
            break;

        case CUIAxisAlignment::Center:
            ResultRect.Top = Bounds.Top + (Bounds.Bottom - Bounds.Top - (SourceRect.Bottom - SourceRect.Top)) / 2 + SourceRect.Top;
            ResultRect.Bottom = ResultRect.Top + (SourceRect.Bottom - SourceRect.Top);
            break;

        case CUIAxisAlignment::Trailing:
            ResultRect.Top = Bounds.Bottom - (SourceRect.Bottom - SourceRect.Top) - SourceRect.Top;
            ResultRect.Bottom = ResultRect.Top + (SourceRect.Bottom - SourceRect.Top);
            break;
        }

        return ResultRect;
    }

    virtual Void Layout(CUIRect Bounds) {
        /*
        static Float32 Anchors[] = { 0.0f, 0.5f, 1.0f, 0.0f };
        Anchor.X = Anchors[AlignmentX];
        Anchor.Y = Anchors[AlignmentY];

        if (AlignmentX == CUIAxisAlignment::Stretch) {
            Position.X = Bounds.Left;
            Scale.X = (Bounds.Right - Bounds.Left) / Size.X;
        }

        if (AlignmentY == CUIAxisAlignment::Stretch) {
            Position.Y = Bounds.Top;
            Scale.Y = (Bounds.Bottom - Bounds.Top) / Size.Y;
        }

        CUIRect Rect;
        Rect.Left = Position.X - Anchor.X * Size.X * Scale.X;
        Rect.Top = Position.Y - Anchor.Y * Size.Y * Scale.Y;
        Rect.Right = Rect.Left + Size.X * Scale.X;
        Rect.Bottom = Rect.Top + Size.Y * Scale.Y;

        for (auto Child : Children) {
            Child->Layout(Rect);
        }*/
    }

    virtual Void Update(CUIInputState* InputState) {
        if (Contains(CUIPoint(InputState->MouseX, InputState->MouseY))) {
            if (InputState->KeyStates[VK_LBUTTON]) {
                State = CUIControlState::Active;
            } else {
                State = CUIControlState::Hover;
            }
        }
        else {
            State = CUIControlState::Normal;
        }

        for (auto Child : Children) {
            if (!Child->Active) continue;

            Child->Update(InputState);
        }
    }

    virtual Void Render(CRenderer* Renderer) { }
};