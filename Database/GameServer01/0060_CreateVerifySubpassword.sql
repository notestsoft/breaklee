CREATE PROCEDURE VerifySubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InExpirationInHours INT,
    IN InPassword VARCHAR(11),
    OUT OutSuccess INT,
    OUT OutFailureCount TINYINT,
    OUT OutIsLocked INT,
    OUT OutType INT
)
BEGIN
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempMaxSubpasswordFailureCount INT DEFAULT 5;
    DECLARE CurrentFailureCount TINYINT DEFAULT 0;
    DECLARE IsLocked BOOLEAN DEFAULT FALSE;

    -- Initialize output parameters
    SET OutSuccess = 0;
    SET OutFailureCount = 0;
    SET OutIsLocked = 0;
    SET OutType = InType;

    -- Retrieve the stored password, failure count, and lock status for the account and type
    SELECT Password, FailureCount, Locked 
    INTO TempStoredPassword, CurrentFailureCount, IsLocked
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = InType;

    -- Update the lock status in the output parameter
    SET OutIsLocked = IF(IsLocked, 1, 0);

    -- Check if the account is locked
    IF IsLocked THEN
        SET OutSuccess = 0;
        SET OutFailureCount = CurrentFailureCount;
    ELSE
        -- Determine if the subpassword is set
        IF TempStoredPassword IS NOT NULL AND TempStoredPassword != '' THEN
            -- Validate the provided password
            IF InPassword != TempStoredPassword THEN
                -- Increment failure count if password check fails
                SET CurrentFailureCount = CurrentFailureCount + 1;

                UPDATE Subpasswords
                SET FailureCount = CurrentFailureCount,
                    UpdatedAt = UNIX_TIMESTAMP()
                WHERE AccountID = InAccountID AND Type = InType;

                -- Check if the failure count exceeds the maximum allowed
                IF CurrentFailureCount >= TempMaxSubpasswordFailureCount THEN
                    -- Lock the account if failure count exceeds the maximum
                    UPDATE Subpasswords
                    SET Locked = TRUE,
                        UpdatedAt = UNIX_TIMESTAMP()
                    WHERE AccountID = InAccountID AND Type = InType;

                    -- Update the lock status in the output parameter
                    SET OutIsLocked = 1;
                END IF;

                -- Indicate failure
                SET OutSuccess = 0;
                SET OutFailureCount = CurrentFailureCount;
            ELSE
                -- Reset failure count on success and update RequestTimeout
                UPDATE Subpasswords
                SET FailureCount = 0,
                    RequestTimeout = UNIX_TIMESTAMP() + (InExpirationInHours * 3600),
                    UpdatedAt = UNIX_TIMESTAMP()
                WHERE AccountID = InAccountID AND Type = InType;

                -- Indicate success
                SET OutSuccess = 1;
                SET OutFailureCount = 0;
            END IF;
        ELSE
            -- Handle case where the password is not set
            SET OutSuccess = 0;
        END IF;
    END IF;
END;
