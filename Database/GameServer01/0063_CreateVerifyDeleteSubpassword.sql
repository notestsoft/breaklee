CREATE PROCEDURE VerifyDeleteSubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InPassword VARCHAR(11),  -- MAX_SUBPASSWORD_LENGTH is 10 + 1 for null-termination
    OUT OutSuccess INT,
    OUT OutFailureCount TINYINT
)
BEGIN
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE TempFailureCount TINYINT DEFAULT 0;  -- Use TINYINT for failure count
    DECLARE TempIsLocked BOOLEAN DEFAULT FALSE;

    -- Initialize output parameters
    SET OutSuccess = 0; 
    SET OutFailureCount = 0;

    -- Retrieve the stored password and lock status for the given account and type
    SELECT Password, Locked, FailureCount INTO TempStoredPassword, TempIsLocked, TempFailureCount
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = InType;

    -- Determine if the subpassword is set
    SET TempIsSubpasswordSet = CHAR_LENGTH(TempStoredPassword) > 0;

    -- Check if the provided password is correct and the account is not locked
    IF TempIsSubpasswordSet AND NOT TempIsLocked 
        AND InPassword = TempStoredPassword THEN
        
        -- Reset failure count and mark success
        SET OutSuccess = 1;
        SET OutFailureCount = 0;
    ELSE
        -- Increment failure count and ensure it fits within TINYINT range
        SET TempFailureCount = LEAST(255, TempFailureCount + 1);

        -- Update the failure count and lock status if needed
        UPDATE Subpasswords
        SET FailureCount = TempFailureCount,
            Locked = IF(TempFailureCount >= 5, TRUE, Locked),  -- Assuming 5 as max failure count before lock
            UpdatedAt = UNIX_TIMESTAMP()
        WHERE AccountID = InAccountID AND Type = InType;

        -- Set the output parameters for failure
        SET OutSuccess = 0;
        SET OutFailureCount = TempFailureCount;
    END IF;
END;
