RUNTIME_DATA_FILE_BEGIN(craft.enc)

RUNTIME_DATA_TYPE_BEGIN(ProfessionalCraftEnergy, "cabal_craft_system.craft_energy_base")
	RUNTIME_DATA_PROPERTY(Int32, BaseEnergy, "base_energy")
RUNTIME_DATA_TYPE_END(ProfessionalCraftEnergy)
RUNTIME_DATA_TYPE_INDEX(ProfessionalCraftEnergy, Int32, BaseEnergy)

RUNTIME_DATA_TYPE_BEGIN(ProfessionalCraftBase, "cabal_craft_system.craft_level_base")
	RUNTIME_DATA_PROPERTY(Int32, Level, "craft_level")
	RUNTIME_DATA_PROPERTY(Int32, EXPMin, "craft_exp_min")
	RUNTIME_DATA_PROPERTY(Int32, EXPMax, "craft_exp_max")
	RUNTIME_DATA_PROPERTY(Int64, LevelExpandCost, "craft_level_expansioncost")
	RUNTIME_DATA_PROPERTY(Int32, EnergyBonus, "craft_energy_bonus")
	RUNTIME_DATA_PROPERTY(Int32, GetEXPMinGrade, "craft_getexp_mingrade")
RUNTIME_DATA_TYPE_END(ProfessionalCraftBase)
RUNTIME_DATA_TYPE_INDEX(ProfessionalCraftBase, Int32, Level)

RUNTIME_DATA_TYPE_BEGIN(ProfessionalCraftFormula, "cabal_craft_system.craft_fomula.fomula")
	RUNTIME_DATA_PROPERTY(Int32, ItemCategory, "craft_item_category")
	RUNTIME_DATA_PROPERTY(Int32, EXPGainBase, "craft_getexp_base")
	RUNTIME_DATA_PROPERTY(Int32, EnergyUseBase, "craft_useenergy_base")
	RUNTIME_DATA_PROPERTY(Int64, ExpansionItemIndex, "craft_category_expansion_itemindex")
	RUNTIME_DATA_PROPERTY(Int32, CategoryGroup, "craft_category_group")
RUNTIME_DATA_TYPE_END(ProfessionalCraftFormula)
RUNTIME_DATA_TYPE_INDEX(ProfessionalCraftFormula, Int32, ItemCategory)

RUNTIME_DATA_TYPE_BEGIN(ProfessionalCraftCategoryInfo, "cabal_craft_system.craft_info.category_info")
	RUNTIME_DATA_PROPERTY(Int32, ItemCategory, "craft_item_category")
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, ItemGrade, "craft_item_grade", 2, ':')
	RUNTIME_DATA_PROPERTY(Int32, ItemMainOption, "craft_item_mainoption")
RUNTIME_DATA_TYPE_END(ProfessionalCraftCategoryInfo)
RUNTIME_DATA_TYPE_INDEX(ProfessionalCraftCategoryInfo, Int32, ItemCategory)

RUNTIME_DATA_TYPE_BEGIN(ProfessionalCraftRecipe, "cabal_craft_system.craft_recipe.recipe")
	RUNTIME_DATA_PROPERTY(Int32, CraftCode, "craft_code")
	RUNTIME_DATA_PROPERTY(Int32, ItemCategory, "craft_item_category")
	RUNTIME_DATA_PROPERTY_ARRAY(Int32, ItemGrade, "craft_item_grade", 2, ':')
	RUNTIME_DATA_PROPERTY(Int32, ItemMainOption, "craft_item_mainoption")
	RUNTIME_DATA_TYPE_BEGIN_CHILD(ProfessionalCraftRecipeMaterial, "material")
		RUNTIME_DATA_PROPERTY(UInt64, ItemIndex, "craft_material_itemindex")
		RUNTIME_DATA_PROPERTY(UInt64, ItemOption, "craft_material_itemoption")
		RUNTIME_DATA_PROPERTY(Int32, ItemCount, "craft_material_itemcount")
	RUNTIME_DATA_TYPE_END_CHILD(ProfessionalCraftRecipeMaterial)
RUNTIME_DATA_TYPE_END(ProfessionalCraftRecipe)
RUNTIME_DATA_TYPE_INDEX(ProfessionalCraftRecipe, Int32, CraftCode)

RUNTIME_DATA_FILE_END