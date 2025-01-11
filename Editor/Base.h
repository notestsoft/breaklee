#pragma once

#include <CoreLib/CoreLib.h>
#include <TextureLib/TextureLib.h>

#include <float.h>
#include <raylib.h>
#include <rcamera.h>
#include <rlgl.h>

#undef far
#undef near
#include <raymath.h>

#define RAYGUI_TEXTSPLIT_MAX_ITEMS 32768
#define RAYGUI_TEXTSPLIT_MAX_TEXT_SIZE 524288
#include "raygui/raygui.h"

#define TARGET_NAME "BreakleeStudio"
#define UI_TEXT_LENGTH 64

typedef struct _EditorContext* EditorContextRef;