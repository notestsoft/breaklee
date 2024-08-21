CREATE PROCEDURE DeleteCharacter (
    IN InAccountID INT,
    IN InCharacterID INT,
    OUT OutSuccess BOOLEAN
)
BEGIN
    -- Initialize the success flag
    SET OutSuccess = 0;

    -- Perform the DELETE statement
    DELETE FROM Characters 
    WHERE 
        AccountID = InAccountID AND 
        CharacterID = InCharacterID;

    -- Check if any rows were affected
    IF ROW_COUNT() > 0 THEN
        SET OutSuccess = 1;  -- Deletion successful
    ELSE
        SET OutSuccess = 0;  -- No rows were deleted, possibly invalid AccountID or CharacterID
    END IF;
END;
