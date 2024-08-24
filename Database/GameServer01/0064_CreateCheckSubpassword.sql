CREATE PROCEDURE CheckSubpassword(
    IN InAccountID INT,
    OUT OutIsVerificationRequired INT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE TempIsVerifiedSession BOOLEAN DEFAULT FALSE;
    DECLARE TempStoredPassword VARCHAR(11);
    DECLARE TempRequestTimeout BIGINT UNSIGNED;

    -- Initialize the output parameter
    SET OutIsVerificationRequired = 0;

    -- Fetch the subpassword and request timeout for the account
    SELECT Password, RequestTimeout
    INTO TempStoredPassword, TempRequestTimeout
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = 1; -- Assuming Type 1 for character subpasswords

    -- Check if a subpassword is set
    SET TempIsSubpasswordSet = CHAR_LENGTH(TempStoredPassword) > 0;

    -- Check if the request timeout is still valid
    SET TempIsVerifiedSession = UNIX_TIMESTAMP() < TempRequestTimeout;

    -- Determine if verification is required
    IF TempIsSubpasswordSet AND NOT TempIsVerifiedSession THEN
        SET OutIsVerificationRequired = 1;
    ELSE
        SET OutIsVerificationRequired = 0;
    END IF;
END;
