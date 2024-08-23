CREATE PROCEDURE InsertSubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InPassword VARCHAR(10),
    IN InQuestion INT,
    IN InAnswer VARCHAR(16),
    OUT OutSuccess INT,
    OUT OutType INT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;

    -- Initialize output parameters
    SET OutSuccess = 0;
    SET OutType = InType;

    -- Only proceed if the type is CHARACTER_SUBPASSWORD_TYPE_CHARACTER (assumed to be 1)
    IF InType = 1 THEN
        -- Fetch the existing subpassword status
        SELECT CHAR_LENGTH(CharacterPassword) > 0 INTO TempIsSubpasswordSet
        FROM Accounts
        WHERE AccountID = InAccountID;

        -- Check if a subpassword is already set or if the provided credentials do not meet the length requirements
        IF TempIsSubpasswordSet 
            OR CHAR_LENGTH(InPassword) < 4 
            OR CHAR_LENGTH(InPassword) > 10
            OR CHAR_LENGTH(InAnswer) < 1
            OR CHAR_LENGTH(InAnswer) > 16 THEN
            
            -- Subpassword already set or credentials are too short or too long, return failure
            SET OutSuccess = 0;
        ELSE
            -- Update the account with the new subpassword, question, and answer
            UPDATE Accounts
            SET CharacterPassword = InPassword,
                CharacterQuestion = InQuestion,
                CharacterAnswer = InAnswer,
                UpdatedAt = UNIX_TIMESTAMP()
            WHERE AccountID = InAccountID;

            -- Set the success to 1 to indicate success
            SET OutSuccess = 1;
        END IF;
    ELSE
        -- Unsupported type, return failure
        SET OutSuccess = 0;
    END IF;
END;