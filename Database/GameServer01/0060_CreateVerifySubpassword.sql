CREATE PROCEDURE VerifySubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InExpirationInHours INT,
    IN InPassword VARCHAR(11),
    IN InSessionIP VARCHAR(45),
    OUT OutSuccess INT,
    OUT OutFailureCount TINYINT
)
BEGIN
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempMaxSubpasswordFailureCount INT DEFAULT 5;
    DECLARE CurrentFailureCount TINYINT DEFAULT 0;

    -- Initialize output parameters
    SET OutSuccess = 0;
    SET OutFailureCount = 0;

    -- Check if the provided type is 1
    IF InType = 1 THEN
        -- Retrieve the stored password for the account
        SELECT CharacterPassword INTO TempStoredPassword
        FROM Accounts
        WHERE AccountID = InAccountID;

        -- Determine if the subpassword is set
        IF TempStoredPassword IS NOT NULL AND TempStoredPassword != '' THEN
            -- Validate the provided password
            IF InPassword != TempStoredPassword THEN
                -- Increment failure count if password check fails
                SELECT SubpasswordFailureCount INTO CurrentFailureCount
                FROM Accounts
                WHERE AccountID = InAccountID;

                SET CurrentFailureCount = CurrentFailureCount + 1;

                UPDATE Accounts
                SET SubpasswordFailureCount = CurrentFailureCount,
                    UpdatedAt = UNIX_TIMESTAMP()
                WHERE AccountID = InAccountID;

                -- Check if the failure count exceeds the maximum allowed
                -- IF CurrentFailureCount >= TempMaxSubpasswordFailureCount THEN
                    -- TODO: Implement account ban logic if necessary
                -- END IF;

                -- Indicate failure
                SET OutSuccess = 0;
                SET OutFailureCount = CurrentFailureCount;
            ELSE
                -- Reset failure count and update session details on success
                UPDATE Accounts
                SET SubpasswordFailureCount = 0,
                    AddressIP = InSessionIP,
                    SessionTimeout = UNIX_TIMESTAMP() + (InExpirationInHours * 3600),
                    UpdatedAt = UNIX_TIMESTAMP()
                WHERE AccountID = InAccountID;

                -- Indicate success
                SET OutSuccess = 1;
                SET OutFailureCount = 0;
            END IF;
        ELSE
            -- Handle case where password is not set
            SET OutSuccess = 0;
        END IF;
    END IF;
END;
