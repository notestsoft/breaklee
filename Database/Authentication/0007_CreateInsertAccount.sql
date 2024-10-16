CREATE PROCEDURE InsertAccount(
    IN InUsername VARCHAR(16),
    IN InEmail VARCHAR(255),
    IN InPassword VARCHAR(255),
    IN InIterations INT
)
BEGIN
    DECLARE TempSalt VARBINARY(16);
    DECLARE TempPasswordHash VARBINARY(80);

    SET TempSalt = UNHEX(SUBSTRING(REPLACE(UUID(), '-', ''), 1, 32)); 

    CALL HashPassword(InPassword, TempSalt, InIterations, TempPasswordHash);

    INSERT INTO Accounts (
        Username, Email, PasswordHash, CreatedAt, UpdatedAt
    )
    VALUES (
        InUsername, InEmail, TempPasswordHash, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()
    );
END;
