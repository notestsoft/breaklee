#include "HonorRank.h"
#include "Runtime.h"

Int32 RTCharacterGetHonorRank(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
	for (Int32 Index = 0; Index < Runtime->Context->HonorLevelFormulaCount; Index += 1) {
		RTDataHonorLevelFormulaRef Formula = &Runtime->Context->HonorLevelFormulaList[Index];
		if (Formula->MinPoint > Character->Data.Info.HonorPoint) continue;
		if (Formula->MaxPoint < Character->Data.Info.HonorPoint) continue;

		return Formula->Rank;
	}

	return 0;
}
