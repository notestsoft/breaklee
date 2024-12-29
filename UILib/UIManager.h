#pragma once

#include "Base.h"

EXTERN_C_BEGIN

UIManagerRef UIManagerCreate(
	AllocatorRef Allocator
);

Void UIManagerDestroy(
	UIManagerRef UIManager
);

EXTERN_C_END
