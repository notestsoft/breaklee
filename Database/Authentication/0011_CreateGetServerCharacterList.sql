CREATE PROCEDURE GetServerCharacterList (
    IN InAccountID INT,
    OUT OutResultCount INT
)
BEGIN
    DECLARE TempResultCount INT DEFAULT 0;

    CREATE TEMPORARY TABLE TempCharacters (
        ServerID INT,
        CharacterCount INT
    );

    INSERT INTO TempCharacters (ServerID, CharacterCount)
    SELECT ServerID, CharacterCount
    FROM Characters
    WHERE AccountID = InAccountID
    ORDER BY ServerID;

    SELECT COUNT(*) INTO TempResultCount FROM TempCharacters;

    SET OutResultCount = TempResultCount;

    SELECT * FROM TempCharacters;

    DROP TEMPORARY TABLE TempCharacters;
END;
