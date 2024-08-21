CREATE PROCEDURE UpdateAccountCharacterPassword (
    IN InAccountID INT,
    IN InCharacterPassword VARCHAR(11),
    IN InCharacterQuestion INT UNSIGNED,
    IN InCharacterAnswer VARCHAR(17),
    OUT OutSuccess BOOLEAN
)
BEGIN
    -- Initialize the success flag
    SET OutSuccess = 0;

    -- Perform the UPDATE statement
    UPDATE Accounts 
    SET 
        CharacterPassword = InCharacterPassword,
        CharacterQuestion = InCharacterQuestion,
        CharacterAnswer = InCharacterAnswer
    WHERE 
        AccountID = InAccountID;

    -- Check if any rows were affected
    IF ROW_COUNT() > 0 THEN
        SET OutSuccess = 1;  -- Update successful
    ELSE
        SET OutSuccess = 0;  -- No rows were updated, possibly invalid AccountID
    END IF;
END;
