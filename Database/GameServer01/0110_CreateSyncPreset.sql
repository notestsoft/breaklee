CREATE PROCEDURE SyncPreset (
    IN InCharacterID INT,
    IN InConfigurationData BLOB,
    IN InActiveEquipmentPresetIndex INT,
    IN InActiveAnimaMasteryPresetIndex INT
)
BEGIN
    -- Update the existing record in the Preset table
    UPDATE Preset
    SET
        ConfigurationData = InConfigurationData,
        ActiveEquipmentPresetIndex = InActiveEquipmentPresetIndex,
        ActiveAnimaMasteryPresetIndex = InActiveAnimaMasteryPresetIndex
    WHERE CharacterID = InCharacterID;
END;
