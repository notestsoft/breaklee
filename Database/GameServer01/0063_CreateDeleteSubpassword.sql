CREATE PROCEDURE DeleteSubpassword(
    IN InAccountID INT,
    IN InType INT,
    OUT OutSuccess INT,
    OUT OutType INT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;

    -- Initialize output parameters
    SET OutSuccess = 0;
    SET OutType = InType;

    -- Fetch the stored subpassword information for the account and type
    SELECT CHAR_LENGTH(Password) > 0 INTO TempIsSubpasswordSet
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = InType;

    -- Check if the subpassword is set
    IF TempIsSubpasswordSet THEN
        -- Delete the subpassword, question, and answer
        DELETE FROM Subpasswords
        WHERE AccountID = InAccountID AND Type = InType;

        -- Set success to 1 indicating success
        SET OutSuccess = 1;
    ELSE
        -- Subpassword not set, success remains 0
        SET OutSuccess = 0;
    END IF;
END;
