#pragma once

#include "Base.h"

struct CUIColor {
    UInt8 R;
    UInt8 G;
    UInt8 B;
    UInt8 A;

    CUIColor() : R(0), G(0), B(0), A(0) {
    }

    CUIColor(
        UInt8 R,
        UInt8 G,
        UInt8 B,
        UInt8 A
    );

    UInt32 GetValue();
};
