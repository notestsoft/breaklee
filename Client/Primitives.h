#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef struct _Vector2 {
    Float32 X;
    Float32 Y;
} Vector2;

typedef struct _Vector3 {
    Float32 X;
    Float32 Y;
    Float32 Z;
} Vector3;

typedef struct _Vector4 {
    Float32 X;
    Float32 Y;
    Float32 Z;
    Float32 W;
} Vector4;

typedef struct _Quaternion {
    Float32 X;
    Float32 Y;
    Float32 Z;
    Float32 W;
} Quaternion;

typedef struct _Matrix4x4 {
    struct { Vector4 Column; } Row[4];
} Matrix4;

typedef struct _Size {
    Int32 Width;
    Int32 Height;
} Size;

typedef struct _UIRect {
    Int32 X;
    Int32 Y;
    Int32 Width;
    Int32 Height;
} UIRect;

typedef struct _UIColor {
    union {
        struct {
            UInt8 R;
            UInt8 G;
            UInt8 B;
            UInt8 A;
        };
        DWORD Value;
    };
} UIColor;

EXTERN_C_END