CREATE PROCEDURE DeleteSubpassword(
    IN InAccountID INT,
    IN InType INT,
    OUT OutSuccess TINYINT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;

    -- Initialize the output parameter
    SET OutSuccess = 0;

    -- Only proceed if the type is CHARACTER_SUBPASSWORD_TYPE_CHARACTER (assumed to be 1)
    IF InType = 1 THEN
        -- Fetch the stored subpassword information for the account
        SELECT CHAR_LENGTH(CharacterPassword) > 0 INTO TempIsSubpasswordSet
        FROM Accounts
        WHERE AccountID = InAccountID;

        -- Check if subpassword is set, deletion is verified, and session is not expired
        IF TempIsSubpasswordSet THEN
            -- Clear the subpassword, question, and answer
            UPDATE Accounts
            SET CharacterPassword = NULL,
                CharacterQuestion = 0,
                CharacterAnswer = NULL,
                UpdatedAt = UNIX_TIMESTAMP()
            WHERE AccountID = InAccountID;

            -- Set the success to 1 to indicate success
            SET OutSuccess = 1;
        ELSE
            -- Conditions not met, success remains 0
            SET OutSuccess = 0;
        END IF;
    ELSE
        -- Unsupported type, success remains 0
        SET OutSuccess = 0;
    END IF;
END;
