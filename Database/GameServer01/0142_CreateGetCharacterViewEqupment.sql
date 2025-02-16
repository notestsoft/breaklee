CREATE PROCEDURE GetCharacterViewEqData(
    IN InCharacterID INT
)
BEGIN
    SELECT 
        characters.Name AS 'Name',
        Characters.Level AS 'Level',
        CASE
        WHEN (Style & 0x0F) >= 10 THEN (Style & 0x0F) - 8
        ELSE (Style & 0x0F)
	    END AS 'Style',
        Settings.OptionsDataLength,
        Equipment.EquipmentSlotCount,
        Equipment.EquipmentSlotData,
        Settings.OptionsData
    FROM Characters
    LEFT JOIN Equipment ON Characters.CharacterID = Equipment.CharacterID
    LEFT JOIN Settings ON Characters.AccountID = Settings.AccountID
    WHERE Characters.CharacterID = InCharacterID;
END