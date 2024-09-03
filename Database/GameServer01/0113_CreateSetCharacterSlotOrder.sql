CREATE PROCEDURE SetCharacterSlotOrder (
    IN InAccountID INT,
    IN InCharacterSlotOrder BIGINT UNSIGNED
)
BEGIN
    UPDATE Accounts
    SET
        CharacterSlotOrder = InCharacterSlotOrder
    WHERE AccountID = InAccountID;
END;