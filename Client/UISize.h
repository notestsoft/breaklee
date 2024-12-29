#pragma once

#include "Base.h"

struct CUISize {
    Float32 X;
    Float32 Y;

    CUISize() :
        X(0),
        Y(0) {
    }

    CUISize(
        Float32 X,
        Float32 Y
    ) : X(X),
        Y(Y) {
    }
};
