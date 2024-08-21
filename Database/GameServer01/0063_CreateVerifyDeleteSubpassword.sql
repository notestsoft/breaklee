CREATE PROCEDURE VerifyDeleteSubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InPassword VARCHAR(11), -- MAX_SUBPASSWORD_LENGTH is 10 + 1 for null-termination
    OUT OutSuccess TINYINT,
    OUT OutFailureCount TINYINT
)
BEGIN
    DECLARE TempSubpasswordLength INT;
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempMaxSubpasswordFailureCount INT DEFAULT 5; -- Adjust based on your configuration

    -- Initialize output parameters
    SET OutSuccess = 0; 
    SET OutFailureCount = 0;

    -- Only proceed if the type is CHARACTER_SUBPASSWORD_TYPE_CHARACTER
    IF InType = 1 THEN
        -- Fetch the stored character password for the account
        SELECT CharacterPassword INTO TempStoredPassword
        FROM Accounts
        WHERE AccountID = InAccountID;

        -- Check if the stored password is set and calculate its length
        SET TempSubpasswordLength = CHAR_LENGTH(TempStoredPassword);
        SET TempIsSubpasswordSet = TempSubpasswordLength > 0;

        -- Check if the provided password is valid
        IF TempIsSubpasswordSet = FALSE 
            OR CHAR_LENGTH(InPassword) < TempSubpasswordLength 
            OR InPassword != TempStoredPassword THEN
            
            -- Increment the failure count
            SELECT @FailureCount := SubpasswordFailureCount FROM Accounts WHERE AccountID = InAccountID;
            SET OutFailureCount = @FailureCount + 1;

            -- Update the failure count in the database
            UPDATE Accounts
            SET SubpasswordFailureCount = OutFailureCount,
                UpdatedAt = UNIX_TIMESTAMP()
            WHERE AccountID = InAccountID;

            -- Set the success to 0 to indicate failure
            SET OutSuccess = 0;
        ELSE
            -- If password is correct, reset failure count and mark the deletion as verified
            SET OutFailureCount = 0;

            UPDATE Accounts
            SET SubpasswordFailureCount = 0,
                UpdatedAt = UNIX_TIMESTAMP()
            WHERE AccountID = InAccountID;

            -- Set the success to 1 to indicate success
            SET OutSuccess = 1;
        END IF;
    END IF;
END;
