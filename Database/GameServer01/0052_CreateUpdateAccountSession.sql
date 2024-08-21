CREATE PROCEDURE UpdateAccountSession (
    IN InAccountID INT,
    IN InAddressIP VARCHAR(45),
    IN InSessionTimeout BIGINT UNSIGNED,
    OUT OutSuccess BOOLEAN
)
BEGIN
    -- Initialize the success flag
    SET OutSuccess = 0;

    -- Perform the UPDATE statement
    UPDATE Accounts 
    SET 
        AddressIP = InAddressIP,
        SessionTimeout = InSessionTimeout
    WHERE 
        AccountID = InAccountID;

    -- Check if any rows were affected
    IF ROW_COUNT() > 0 THEN
        SET OutSuccess = 1;  -- Update successful
    ELSE
        SET OutSuccess = 0;  -- No rows were updated, possibly invalid AccountID
    END IF;
END;
