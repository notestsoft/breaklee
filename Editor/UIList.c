#include "UIList.h"

struct _UIListItem {
	Char Name[UI_TEXT_LENGTH];
	UInt8 Data[0];
};

struct _UIList {
	AllocatorRef Allocator;
	Int32 ItemDataLength;
	Int32 ScrollIndex;
	Int32 ActiveIndex;
	ArrayRef Names;
	ArrayRef Items;
	ArrayRef FilteredNames;
	ArrayRef FilteredItems;
};

UIListRef UIListCreate(
	AllocatorRef Allocator,
	Int32 ItemDataLength
) {
	UIListRef List = (UIListRef)AllocatorAllocate(Allocator, sizeof(struct _UIList));
	if (!List) Fatal("Memory allocation failed!");

	List->Allocator = Allocator;
	List->ItemDataLength = ItemDataLength;
	List->ScrollIndex = 0;
	List->ActiveIndex = -1;
	List->Names = ArrayCreateEmpty(Allocator, sizeof(Char), 8);
	List->Items = ArrayCreateEmpty(Allocator, sizeof(struct _UIListItem) + ItemDataLength, 8);
	List->FilteredNames = ArrayCreateEmpty(Allocator, sizeof(Char), 8);
	List->FilteredItems = ArrayCreateEmpty(Allocator, sizeof(struct _UIListItem) + ItemDataLength, 8);
	Char Terminator = '\0';
	ArrayAppendElement(List->Names, &Terminator);
	ArrayAppendElement(List->FilteredNames, &Terminator);
	return List;
}

Void UIListDestroy(
	UIListRef List
) {
	ArrayDestroy(List->Items);
	ArrayDestroy(List->Names);
	ArrayDestroy(List->FilteredItems);
	ArrayDestroy(List->FilteredNames);
	AllocatorDeallocate(List->Allocator, List);
}

UIListItemRef UIListAppendItem(
	UIListRef List,
	CString Name,
	Void* Data,
	Int32 DataLength
) {
	assert(strlen(Name) < UI_TEXT_LENGTH);
	assert(DataLength <= List->ItemDataLength);

	UIListItemRef Item = ArrayAppendUninitializedElement(List->Items);
	CStringCopySafe(Item->Name, UI_TEXT_LENGTH, Name);
	if (DataLength > 0) memcpy(Item->Data, Data, DataLength);
	
	CString Names = (CString)ArrayGetElementAtIndex(List->Names, 0);
	Int32 NameLength = ArrayGetElementCount(List->Names) - 1;
	if (NameLength > 0) Names[ArrayGetElementCount(List->Names) - 1] = ';';
	else ArrayRemoveElementAtIndex(List->Names, ArrayGetElementCount(List->Names) - 1);
	ArrayAppendMemory(List->Names, Name, strlen(Name) + 1);

	UIListItemRef FilteredItem = ArrayAppendUninitializedElement(List->FilteredItems);
	CStringCopySafe(FilteredItem->Name, UI_TEXT_LENGTH, Name);
	if (DataLength > 0) memcpy(FilteredItem->Data, Data, DataLength);

	CString FilteredNames = (CString)ArrayGetElementAtIndex(List->FilteredNames, 0);
	Int32 FilteredNameLength = ArrayGetElementCount(List->FilteredNames) - 1;
	if (FilteredNameLength > 0) FilteredNames[ArrayGetElementCount(List->FilteredNames) - 1] = ';';
	else ArrayRemoveElementAtIndex(List->FilteredNames, ArrayGetElementCount(List->FilteredNames) - 1);
	ArrayAppendMemory(List->FilteredNames, Name, strlen(Name) + 1);

	return Item;
}

UInt8* UIListItemGetData(
	UIListItemRef ListItem
) {
	return &ListItem->Data[0];
}

Void UIListFilter(
	UIListRef List,
	CString SearchTerm
) {
	ArrayRemoveAllElements(List->FilteredItems, true);
	ArrayRemoveAllElements(List->FilteredNames, true);

	Char Terminator = '\0';
	ArrayAppendElement(List->FilteredNames, &Terminator);

	for (Int Index = 0; Index < ArrayGetElementCount(List->Items); Index += 1) {
		UIListItemRef Item = (UIListItemRef)ArrayGetElementAtIndex(List->Items, Index);
		if (strlen(SearchTerm) > 0 && !strstr(Item->Name, SearchTerm)) continue;

		UIListItemRef FilteredItem = ArrayAppendUninitializedElement(List->FilteredItems);
		CStringCopySafe(FilteredItem->Name, UI_TEXT_LENGTH, Item->Name);
		if (List->ItemDataLength > 0) memcpy(FilteredItem->Data, Item->Data, List->ItemDataLength);

		CString FilteredNames = (CString)ArrayGetElementAtIndex(List->FilteredNames, 0);
		Int32 FilteredNameLength = ArrayGetElementCount(List->FilteredNames) - 1;
		if (FilteredNameLength > 0) FilteredNames[ArrayGetElementCount(List->FilteredNames) - 1] = ';';
		else ArrayRemoveElementAtIndex(List->FilteredNames, ArrayGetElementCount(List->FilteredNames) - 1);
		ArrayAppendMemory(List->FilteredNames, Item->Name, strlen(Item->Name) + 1);
	}
}

Bool UIListDraw(
	EditorContextRef Context,
	UIListRef List,
	Rectangle Frame,
	UIListItemRef* Selection
) {
	if (ArrayGetElementCount(List->FilteredItems) < 1) return false;

	Int32 PreviousActiveIndex = List->ActiveIndex;
	CString Names = (CString)ArrayGetElementAtIndex(List->FilteredNames, 0);
	GuiListView(Frame, Names, &List->ScrollIndex, &List->ActiveIndex);
	if (PreviousActiveIndex != List->ActiveIndex && List->ActiveIndex >= 0) {
		*Selection = (UIListItemRef)ArrayGetElementAtIndex(List->FilteredItems, List->ActiveIndex);
		return true;
	}

	return false;
}
