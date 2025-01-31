CREATE PROCEDURE SyncUpgrade (
    IN InCharacterID INT,
    IN InPoint INT,
    IN InTimestamp BIGINT UNSIGNED
)
BEGIN
    UPDATE Upgrade
    SET
        Point = InPoint,
        Timestamp = InTimestamp
    WHERE CharacterID = InCharacterID;
END;