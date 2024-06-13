#pragma once

#include "Base.h"

class CLocalization {
	CMap<String, String> Labels;
	CMap<String, String> Words;

public:
	CLocalization();

	~CLocalization();

	Void InsertLabel(
		String Key,
		String Value
	);

	Void InsertWord(
		String Key,
		String Value
	);

	String LocalizeLabel(
		String Key
	);

	String LocalizeWord(
		String Key
	);
};