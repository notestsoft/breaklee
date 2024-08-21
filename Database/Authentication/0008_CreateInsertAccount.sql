CREATE PROCEDURE InsertAccount(
    IN InUsername VARCHAR(16),
    IN InEmail VARCHAR(255),
    IN InPasswordHash VARBINARY(128)
)
BEGIN
    INSERT INTO Accounts (
        Username, Email, PasswordHash, CreatedAt, UpdatedAt
    )
    VALUES (
        InUsername, InEmail, InPasswordHash, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()
    );
END;
