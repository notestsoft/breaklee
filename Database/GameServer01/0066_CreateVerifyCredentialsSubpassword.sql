CREATE PROCEDURE VerifyCredentialsSubpassword(
    IN InAccountID INT,
    IN InPassword VARCHAR(11),
    OUT OutSuccess TINYINT
)
BEGIN
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;

    -- Initialize output parameter
    SET OutSuccess = 0;

    -- Fetch the stored password for the account and type (assumed to be type 1 for character subpasswords)
    SELECT Password INTO TempStoredPassword
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = 1; -- Assuming Type 1 is for character subpasswords

    -- Check if the password is set and not empty
    SET TempIsSubpasswordSet = CHAR_LENGTH(TempStoredPassword) > 0;

    -- Verify the provided password against the stored password
    IF TempIsSubpasswordSet AND InPassword = TempStoredPassword THEN
        SET OutSuccess = 1; -- Password is correct
    ELSE
        SET OutSuccess = 0; -- Password is incorrect or not set
    END IF;
END;
