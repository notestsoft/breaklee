CREATE PROCEDURE GetCharacterList (
    IN InAccountID INT
)
BEGIN
    -- Select and return the character list directly
    SELECT 
        Characters.SlotIndex,
        Characters.CharacterID, 
        Characters.CreatedAt, 
        Characters.Style, 
        Characters.Level,
        OverlordMastery.Level AS OverlordLevel, 
        MythMastery.Rebirth AS MythRebirth, 
        MythMastery.HolyPower AS MythHolyPower, 
        MythMastery.Level AS MythLevel,
        Characters.SkillRank, 
        Characters.Nation AS NationMask, 
        Characters.Name, 
        Characters.HonorPoint, 
        Costume.ActivePageIndex AS CostumeActivePageIndex,
        Costume.AppliedSlotData AS CostumeAppliedSlots,
        Characters.Currency, 
        Characters.WorldIndex, 
        Characters.X AS PositionX, 
        Characters.Y AS PositionY, 
        Equipment.EquipmentSlotCount, 
        Equipment.EquipmentSlotData, 
        Appearance.EquipmentSlotData AS EquipmentAppearanceSlotData
    FROM Characters
    LEFT JOIN Appearance ON Characters.CharacterID = Appearance.CharacterID
    LEFT JOIN MythMastery ON Characters.CharacterID = MythMastery.CharacterID
    LEFT JOIN OverlordMastery ON Characters.CharacterID = OverlordMastery.CharacterID
    LEFT JOIN Costume ON Characters.CharacterID = Costume.CharacterID
    LEFT JOIN Equipment ON Characters.CharacterID = Equipment.CharacterID
    WHERE Characters.AccountID = InAccountID;
END;
