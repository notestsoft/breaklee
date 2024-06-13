#pragma once

#include "Base.h"
#include "UIPoint.h"

struct CUIRect {
    Int32 Left;
    Int32 Top;
    Int32 Right;
    Int32 Bottom;

    CUIRect() : 
        Left(0), 
        Top(0), 
        Right(0),
        Bottom(0) {
    }

    CUIRect(
        Int32 Left,
        Int32 Top,
        Int32 Right,
        Int32 Bottom
    ) : Left(Left), 
        Top(Top), 
        Right(Right), 
        Bottom(Bottom) {
    }

    Bool Contains(CUIPoint Point) {
        return !(Point.X < Left || Point.X > Right || Point.Y < Top || Point.Y > Bottom);
    }
};
