CREATE PROCEDURE VerifyPassword(
    IN InPassword VARCHAR(255),
    IN InHash VARBINARY(80),
    IN InIterations INT,
    OUT OutIsValid BOOLEAN
)
BEGIN
    DECLARE TempSalt VARBINARY(16);
    DECLARE TempStoredHash VARBINARY(64);
    DECLARE TempHash VARBINARY(64); 
    DECLARE TempPasswordHash VARBINARY(80);

    SET TempSalt = SUBSTRING(InHash, 1, 16);
    SET TempStoredHash = SUBSTRING(InHash, 17, 64);

    CALL HashPassword(InPassword, TempSalt, InIterations, TempPasswordHash);

    IF TempPasswordHash = InHash THEN
        SET OutIsValid = TRUE;
    ELSE
        SET OutIsValid = FALSE;
    END IF;
END;