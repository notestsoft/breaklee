#pragma once

#include "Base.h"

EXTERN_C_BEGIN

typedef Void(*UIViewRenderCallback)(
	EditorContextRef Context,
	Rectangle Frame,
	Void* Content
);

struct _UIView {
	AllocatorRef Allocator;
	DictionaryRef Callbacks;
	Rectangle Frame;
	Int ContentType;
	Void* Content;
};
typedef struct _UIView* UIViewRef;

UIViewRef UIViewCreate(
	AllocatorRef Allocator,
	Rectangle Frame
);

Void UIViewDestroy(
	UIViewRef View
);

Void UIViewSetRenderCallback(
	UIViewRef View,
	Int ContentType,
	UIViewRenderCallback Callback
);

Void UIViewSetContent(
	UIViewRef View,
	Int ContentType,
	Void* Content
);

Void UIViewDraw(
	EditorContextRef Context,
	UIViewRef View
);

EXTERN_C_END