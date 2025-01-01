CREATE PROCEDURE SyncSettings (
    IN InAccountID INT,
    IN InHotKeysDataLength INT UNSIGNED,
    IN InOptionsDataLength INT UNSIGNED,
    IN InMacrosDataLength INT UNSIGNED,
    IN InHotKeysData BLOB,
    IN InOptionsData BLOB,
    IN InMacrosData BLOB
)
BEGIN
    UPDATE Settings
    SET
        HotKeysDataLength = InHotKeysDataLength,
        OptionsDataLength = InOptionsDataLength,
        MacrosDataLength = InMacrosDataLength,
        HotKeysData = InHotKeysData,
        OptionsData = InOptionsData,
        MacrosData = InMacrosData
    WHERE AccountID = InAccountID;
END;
