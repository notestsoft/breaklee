CREATE PROCEDURE CheckSubpassword(
    IN InAccountID INT,
    OUT OutIsVerificationRequired INT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE TempIsVerifiedSession BOOLEAN DEFAULT FALSE;
    DECLARE TempSubpassword VARCHAR(11);
    DECLARE TempSessionTimeout BIGINT;

    -- Initialize the output parameter
    SET OutIsVerificationRequired = 0;

    -- Fetch the subpassword and session timeout for the account
    SELECT CharacterPassword, SessionTimeout
    INTO TempSubpassword, TempSessionTimeout
    FROM Accounts
    WHERE AccountID = InAccountID;

    -- Check if a subpassword is set
    SET TempIsSubpasswordSet = CHAR_LENGTH(TempSubpassword) > 0;

    -- Check if the session is still valid
    SET TempIsVerifiedSession = UNIX_TIMESTAMP() < TempSessionTimeout;

    -- Determine if verification is required
    IF TempIsSubpasswordSet AND NOT TempIsVerifiedSession THEN
        SET OutIsVerificationRequired = 1;
    ELSE
        SET OutIsVerificationRequired = 0;
    END IF;
END;
