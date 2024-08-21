CREATE PROCEDURE GetPremiumService (
    IN InAccountID INT,
    OUT OutServiceType INT UNSIGNED,
    OUT OutStartedAt BIGINT UNSIGNED,
    OUT OutExpiredAt BIGINT UNSIGNED,
    OUT OutSuccess BOOLEAN
)
BEGIN
    -- Initialize the success flag and output variables
    SET OutSuccess = 0;
    SET OutServiceType = NULL;
    SET OutStartedAt = 0;
    SET OutExpiredAt = 0;

    -- Perform the SELECT INTO statement
    SELECT 
        ServiceType, 
        StartedAt, 
        ExpiredAt
    INTO 
        OutServiceType, 
        OutStartedAt, 
        OutExpiredAt
    FROM 
        Services
    WHERE 
        StartedAt <= UNIX_TIMESTAMP() AND 
        ExpiredAt >= UNIX_TIMESTAMP() AND 
        AccountID = InAccountID
    LIMIT 1;  -- Ensures only one record is selected, in case multiple exist

    -- Check if any rows were returned
    IF ROW_COUNT() > 0 THEN
        SET OutSuccess = 1;  -- Selection successful
    ELSE
        SET OutSuccess = 0;  -- No active service found for the account
    END IF;
END;
