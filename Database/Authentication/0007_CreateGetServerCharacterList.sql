CREATE PROCEDURE GetServerCharacterList (
    IN InAccountID INT,
    OUT OutResultCount INT
)
BEGIN
    -- Declare a variable to hold the result count
    DECLARE TempResultCount INT DEFAULT 0;

    -- Create a temporary table to hold the results
    CREATE TEMPORARY TABLE TempCharacters (
        ServerID INT,
        CharacterCount INT
    );

    -- Insert data into the temporary table
    INSERT INTO TempCharacters (ServerID, CharacterCount)
    SELECT ServerID, CharacterCount
    FROM Characters
    WHERE AccountID = InAccountID
    ORDER BY ServerID;

    -- Count the number of results
    SELECT COUNT(*) INTO TempResultCount FROM TempCharacters;

    -- Set the output parameter
    SET OutResultCount = TempResultCount;

    -- Return the result set
    SELECT * FROM TempCharacters;

    -- Cleanup
    DROP TEMPORARY TABLE TempCharacters;
END;
