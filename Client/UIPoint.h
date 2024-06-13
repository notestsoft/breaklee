#pragma once

#include "Base.h"

struct CUIPoint {
    Float32 X;
    Float32 Y;

    CUIPoint() :
        X(0),
        Y(0) {
    }

    CUIPoint(
        Float32 X,
        Float32 Y
    ) : X(X),
        Y(Y) {
    }

    CUIPoint operator+(const CUIPoint& Other) const {
        return CUIPoint(X + Other.X, Y + Other.Y);
    }

    CUIPoint operator-(const CUIPoint& Other) const {
        return CUIPoint(X - Other.X, Y - Other.Y);
    }

    CUIPoint& operator+=(const CUIPoint& Other) {
        X += Other.X;
        Y += Other.Y;
        return *this;
    }

    CUIPoint& operator-=(const CUIPoint& Other) {
        X -= Other.X;
        Y -= Other.Y;
        return *this;
    }
};
