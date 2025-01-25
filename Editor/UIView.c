#include "UIView.h"

UIViewRef UIViewCreate(
	AllocatorRef Allocator,
	Rectangle Frame
) {
	UIViewRef View = (UIViewRef)AllocatorAllocate(Allocator, sizeof(struct _UIView));
	if (!View) Fatal("Memory allocation failed!");
	View->Allocator = Allocator;
	View->Callbacks = IndexDictionaryCreate(Allocator, 8);
	View->Frame = Frame;
	View->ContentType = -1;
	View->Content = NULL;
	return View;
}

Void UIViewDestroy(
	UIViewRef View
) {
	DictionaryDestroy(View->Callbacks);
	AllocatorDeallocate(View->Allocator, View);
}

Void UIViewSetRenderCallback(
	UIViewRef View,
	Int ContentType,
	UIViewRenderCallback Callback
) {
	DictionaryInsert(View->Callbacks, &ContentType, &Callback, sizeof(UIViewRenderCallback));
}

Void UIViewSetContent(
	UIViewRef View,
	Int ContentType,
	Void* Content
) {
	View->ContentType = ContentType;
	View->Content = Content;
}

Void UIViewDraw(
	EditorContextRef Context,
	UIViewRef View
) {
	if (!View->Content) return;

	UIViewRenderCallback* Callback = (UIViewRenderCallback*)DictionaryLookup(View->Callbacks, &View->ContentType);
	if (Callback) (*Callback)(Context, View->Frame, View->Content);
}
