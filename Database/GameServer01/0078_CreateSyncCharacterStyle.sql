CREATE PROCEDURE SyncCharacterStyle (
    IN InCharacterID INT,
    IN InNation TINYINT UNSIGNED,
    IN InWarpMask INT UNSIGNED,
    IN InMapsMask INT UNSIGNED,
    IN InStyle INT UNSIGNED,
    IN InLiveStyle INT UNSIGNED,
    IN InExtendedStyle TINYINT UNSIGNED
)
BEGIN
    UPDATE Characters
    SET
        Nation = InNation,
        WarpMask = InWarpMask,
        MapsMask = InMapsMask,
        Style = InStyle,
        LiveStyle = InLiveStyle,
        ExtendedStyle = InExtendedStyle,
        UpdatedAt = UNIX_TIMESTAMP()
    WHERE CharacterID = InCharacterID;
END;
