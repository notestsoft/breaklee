#include "Localization.h"

CLocalization::CLocalization() : 
	Labels({}),
	Words({}) {
}

CLocalization::~CLocalization() {
}

Void CLocalization::InsertLabel(
	String Key,
	String Value
) {
	this->Labels[Key] = Value;
}

Void CLocalization::InsertWord(
	String Key,
	String Value
) {
	this->Words[Key] = Value;
}

String CLocalization::LocalizeLabel(
	String Key
) {
	auto Iterator = this->Labels.find(Key);
	if (Iterator == this->Labels.end()) return Key;
	return Iterator->second;
}

String CLocalization::LocalizeWord(
	String Key
) {
	auto Iterator = this->Words.find(Key);
	if (Iterator == this->Words.end()) return Key;
	return Iterator->second;
}