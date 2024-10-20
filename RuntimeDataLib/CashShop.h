RUNTIME_DATA_FILE_BEGIN(caz.enc)

RUNTIME_DATA_TYPE_BEGIN(CashDurationService, "cabal_cazshop.cabal_duration_svc.duration_svc")
	RUNTIME_DATA_PROPERTY(Int32, Index, "id")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
RUNTIME_DATA_TYPE_END(CashDurationService)

RUNTIME_DATA_TYPE_BEGIN(CashBlessingBeadService, "cabal_cazshop.cabal_duration_svc.blessingbead_svc")
	RUNTIME_DATA_PROPERTY(Int32, Index, "id")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
RUNTIME_DATA_TYPE_END(CashBlessingBeadService)

RUNTIME_DATA_TYPE_BEGIN(CashProductItemDuration, "cabal_cazshop.cabal_product.item_duration.duration")
	RUNTIME_DATA_PROPERTY(Int32, Index, "id")
	RUNTIME_DATA_PROPERTY(Int32, Day, "day")
	RUNTIME_DATA_PROPERTY(Int32, Hour, "hour")
	RUNTIME_DATA_PROPERTY(Int32, Minute, "min")
RUNTIME_DATA_TYPE_END(CashProductItemDuration)

RUNTIME_DATA_TYPE_BEGIN(CashBenefitInfo, "cabal_cazshop.cabal_benefit.benefit_info.benefit")
	RUNTIME_DATA_PROPERTY(Int32, Group, "group")
	RUNTIME_DATA_PROPERTY(Int32, Index, "id")
	RUNTIME_DATA_PROPERTY(Int32, Type, "type")
	RUNTIME_DATA_PROPERTY(Int32, DurationService, "duration_svc")
	RUNTIME_DATA_PROPERTY(Int32, ForceCode1, "forcecode1")
	RUNTIME_DATA_PROPERTY(Int32, ForceCode2, "forcecode2")
	RUNTIME_DATA_PROPERTY(Int32, ForceCode3, "forcecode3")
	RUNTIME_DATA_PROPERTY(Int32, WarReward, "war_reward")
RUNTIME_DATA_TYPE_END(CashBenefitInfo)

RUNTIME_DATA_TYPE_BEGIN(CashBenefitGroup, "cabal_cazshop.cabal_benefit.benefit_group.group")
	RUNTIME_DATA_PROPERTY(Int32, Index, "id")
RUNTIME_DATA_TYPE_END(CashBenefitGroup)

RUNTIME_DATA_TYPE_BEGIN(CashBlessingBeadCustomizing, "cabal_cazshop.used_bead.Used_BlessingBead.customizing")
	RUNTIME_DATA_PROPERTY(Int32, Index, "bbc_idx")
	RUNTIME_DATA_PROPERTY(Int32, BlessingBeadIndex, "bead_index")
	RUNTIME_DATA_PROPERTY(Int32, BeadPerValue, "bead_per")
RUNTIME_DATA_TYPE_END(CashBlessingBeadCustomizing)

RUNTIME_DATA_TYPE_BEGIN(CashPremiumServiceBuff, "cabal_cazshop.PremiumServiceBuff.PremiumBuff")
	RUNTIME_DATA_PROPERTY(Int32, Group, "Group")
	RUNTIME_DATA_PROPERTY(Int32, ForceCode, "ForceCode")
	RUNTIME_DATA_PROPERTY(Int32, ForceValueType, "ValueType")
	RUNTIME_DATA_PROPERTY(Int32, ForceValue, "ForceVlaue")
RUNTIME_DATA_TYPE_END(CashPremiumServiceBuff)

RUNTIME_DATA_FILE_END
