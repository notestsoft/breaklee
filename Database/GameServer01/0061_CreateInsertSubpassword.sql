CREATE PROCEDURE InsertSubpassword(
    IN InAccountID INT,
    IN InType INT,
    IN InPassword VARCHAR(11),          -- Password max length 10 + 1 for null terminator
    IN InQuestion INT,
    IN InAnswer VARCHAR(128),           -- Answer max length 128
    IN InIsChange INT,
    OUT OutSuccess INT,
    OUT OutIsChange INT,
    OUT OutType INT,
    OUT OutIsLocked INT
)
BEGIN
    DECLARE TempIsSubpasswordSet BOOLEAN DEFAULT FALSE;
    DECLARE IsLocked BOOLEAN DEFAULT NULL;

    -- Initialize output parameters
    SET OutSuccess = 0;
    SET OutIsChange = InIsChange;
    SET OutType = InType;
    SET OutIsLocked = 0;

    -- Retrieve existing subpassword lock status
    SELECT Locked INTO IsLocked
    FROM Subpasswords
    WHERE AccountID = InAccountID AND Type = InType;

    -- Check if IsLocked is NULL and update the output parameter
    IF IsLocked IS NULL THEN
        SET OutIsLocked = 0; -- Assume unlocked if NULL
    ELSE
        SET OutIsLocked = IF(IsLocked, 1, 0);
    END IF;

    -- Only proceed if the account is not locked
    IF IsLocked IS NULL OR NOT IsLocked THEN
        -- Check if subpassword already exists
        SELECT CHAR_LENGTH(Password) > 0 INTO TempIsSubpasswordSet
        FROM Subpasswords
        WHERE AccountID = InAccountID AND Type = InType;

        -- Validate input data
        IF (TempIsSubpasswordSet AND InIsChange = 0)
            OR CHAR_LENGTH(InPassword) < 4
            OR CHAR_LENGTH(InPassword) > 10
            OR CHAR_LENGTH(InAnswer) < 1
            OR CHAR_LENGTH(InAnswer) > 128 THEN
            
            -- Subpassword already set and not a change request, or invalid input
            SET OutSuccess = 0;
        ELSE
            -- Insert or update the subpassword, question, and answer
            INSERT INTO Subpasswords (AccountID, Type, Password, Question, Answer, Locked, CreatedAt, UpdatedAt)
            VALUES (InAccountID, InType, InPassword, InQuestion, InAnswer, FALSE, UNIX_TIMESTAMP(), UNIX_TIMESTAMP())
            ON DUPLICATE KEY UPDATE
                Password = VALUES(Password),
                Question = VALUES(Question),
                Answer = VALUES(Answer),
                UpdatedAt = VALUES(UpdatedAt);

            -- Set the success to 1 to indicate success
            SET OutSuccess = 1;
        END IF;
    ELSE
        -- Account is locked, return failure
        SET OutSuccess = 0;
    END IF;
END;
