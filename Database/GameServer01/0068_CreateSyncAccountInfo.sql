CREATE PROCEDURE SyncAccountInfo (
    IN InAccountID INT,
    IN InCharacterSlotID INT,
    IN InCharacterSlotOrder BIGINT UNSIGNED,
    IN InCharacterSlotOpenMask INT UNSIGNED,
    IN InForceGem INT
)
BEGIN 
    UPDATE Accounts
    SET
        CharacterSlotID = InCharacterSlotID,
        CharacterSlotOrder = InCharacterSlotOrder,
        CharacterSlotOpenMask = InCharacterSlotOpenMask,
        ForceGems = InForceGem,
        UpdatedAt = UNIX_TIMESTAMP()
    WHERE AccountID = InAccountID;
END;
