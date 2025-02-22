CREATE PROCEDURE GetCharacterIDByName(
    IN CharacterName VARCHAR(50)
)
BEGIN
    SELECT CharacterID FROM characters WHERE NAME=TRIM(CharacterName) LIMIT 1;
END