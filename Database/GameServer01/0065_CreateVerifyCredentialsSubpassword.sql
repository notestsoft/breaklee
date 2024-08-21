CREATE PROCEDURE VerifyCredentialsSubpassword(
    IN InAccountID INT,
    IN InPassword VARCHAR(11),
    OUT OutSuccess TINYINT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE TempStoredPassword VARCHAR(11);

    SET OutSuccess = 0;

    SELECT CharacterPassword INTO TempStoredPassword
    FROM Accounts
    WHERE AccountID = InAccountID;

    SET TempIsSubpasswordSet = (TempStoredPassword IS NOT NULL AND TempStoredPassword != '');

    IF TempIsSubpasswordSet = FALSE OR InPassword != TempStoredPassword THEN
        SET OutSuccess = 0;
    ELSE
        SET OutSuccess = 1;
    END IF;
END;