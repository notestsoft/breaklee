#include "StellarMastery.h"

RTDataStellarLineGradeRef RTDataStellarMasteryGetLineGrade(
	RTRuntimeRef Runtime,
	RTDataStellarLineRef StellarLine,
	UInt32 MaterialCount
) {
	RTDataStellarLineGradeRef OutGrade = NULL;
	
	for (UInt32 i = 0; i < StellarLine->StellarLineGradeCount; i++) {
		RTDataStellarLineGradeRef Grade = &StellarLine->StellarLineGradeList[i];
		if (Grade->ItemCount == MaterialCount) {
			OutGrade = Grade;
			break;
		}
	}
	return OutGrade;
}