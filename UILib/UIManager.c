#include "UIElement.h"
#include "UIEvent.h"
#include "UILabel.h"
#include "UIManager.h"
#include "UISprite.h"
#include "UITexture.h"

struct _UIManager {
    AllocatorRef Allocator;
    ArrayRef Textures;
    ArrayRef Events;
    ArrayRef Sprites;
    ArrayRef Labels;
    ArrayRef Elements;
    /*
    DictionaryRef Textures;
    DictionaryRef Events;
    DictionaryRef Sprites;
    DictionaryRef Labels;
    DictionaryRef Elements;
    */
};

UIManagerRef UIManagerCreate(
	AllocatorRef Allocator
) {
    UIManagerRef UIManager = (UIManagerRef)AllocatorAllocate(Allocator, sizeof(struct _UIManager));
    if (!UIManager) Fatal("Memory allocation failed!");

    UIManager->Allocator = Allocator;
    UIManager->Textures = ArrayCreateEmpty(Allocator, sizeof(struct _UITexture), 8);
    UIManager->Events = ArrayCreateEmpty(Allocator, sizeof(struct _UIEvent), 8);
    UIManager->Sprites = ArrayCreateEmpty(Allocator, sizeof(struct _UISprite), 8);
    UIManager->Labels = ArrayCreateEmpty(Allocator, sizeof(struct _UILabel), 8);
    UIManager->Elements = ArrayCreateEmpty(Allocator, sizeof(struct _UIElement), 8);
    return UIManager;
}

Void UIManagerDestroy(
	UIManagerRef UIManager
) {

}
