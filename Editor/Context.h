#pragma once

#include "Base.h"
#include "Config.h"

EXTERN_C_BEGIN

enum {
    EDITOR_STATE_INIT,
};

struct _EditorState {
    Int State;
    union {
        struct { UInt8 Padding; } Init;
    };
};
typedef struct _EditorState EditorState;

struct _EditorContext {
    AllocatorRef Allocator;
    EditorConfig Config;
    EditorState State;
    Camera Camera;
};
typedef struct _EditorContext* EditorContextRef;

EXTERN_C_END
