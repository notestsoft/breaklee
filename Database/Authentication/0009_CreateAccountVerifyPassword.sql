CREATE PROCEDURE AccountVerifyPassword(
    IN InAccountID INT,
    IN InPassword VARCHAR(255),
    IN InIterations INT,
    OUT OutIsValid BOOLEAN
)
BEGIN
    DECLARE TempPasswordHash VARBINARY(80);
    DECLARE TempEmailVerified BOOLEAN;
    DECLARE TempDeletedAt BIGINT UNSIGNED;

    SET OutIsValid = FALSE;

    SELECT PasswordHash, EmailVerified, DeletedAt
    INTO TempPasswordHash, TempEmailVerified, TempDeletedAt
    FROM Accounts
    WHERE AccountID = InAccountID;

    IF TempDeletedAt IS NULL THEN
        CALL VerifyPassword(InPassword, TempPasswordHash, InIterations, OutIsValid);
    END IF;
END