CREATE PROCEDURE GetPasswordHash (
    IN InAccountID INT,
    OUT OutResult TINYINT,
    OUT OutPasswordHash VARBINARY(128)
)
BEGIN
    -- Define default status values
    DECLARE STATUS_VALIDATION_SUCCESS TINYINT DEFAULT 1;
    DECLARE STATUS_VALIDATION_FAILURE TINYINT DEFAULT 0;

    DECLARE TempPasswordHash VARBINARY(128);
    DECLARE TempDeletedAt BIGINT UNSIGNED;

    -- Set default output to failure
    SET OutResult = STATUS_VALIDATION_FAILURE;
    SET OutPasswordHash = NULL;

    -- Retrieve the stored hash and deletion status for the account
    SELECT PasswordHash, DeletedAt
    INTO TempPasswordHash, TempDeletedAt
    FROM Accounts
    WHERE AccountID = InAccountID;

    -- Check if the account exists and is not deleted
    IF TempPasswordHash IS NOT NULL AND TempDeletedAt IS NULL THEN
        -- Set the output values if the account is valid
        SET OutResult = STATUS_VALIDATION_SUCCESS;
        SET OutPasswordHash = TempPasswordHash;
    END IF;

END