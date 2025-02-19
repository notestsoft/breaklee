DELIMITER //
CREATE PROCEDURE GetCharacterIDByName(
    IN CharacterName VARCHAR(50)
)
BEGIN
    SELECT CharacterID FROM characters 
    WHERE NAME = TRIM(CharacterName) 
    LIMIT 1;
END //

DELIMITER ;

DELIMITER //
CREATE PROCEDURE GetCharacterViewEqData(
    IN InCharacterID INT
)
BEGIN
    SELECT 
        characters.Name AS 'Name',
        Characters.Level AS Level,
        MOD(Characters.Style, 8) AS Style,
        Settings.OptionsDataLength,
        Equipment.EquipmentSlotCount,
        Equipment.EquipmentSlotData,
        Settings.OptionsData
    FROM Characters
    LEFT JOIN Equipment ON Characters.CharacterID = Equipment.CharacterID
    LEFT JOIN Settings ON Characters.AccountID = Settings.AccountID
    WHERE Characters.CharacterID = InCharacterID;
END //
DELIMITER ;
