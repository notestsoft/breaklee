CREATE PROCEDURE SyncDailyReset (
    IN InCharacterID INT,
    IN InLastResetTimestamp BIGINT UNSIGNED,
    IN InNextResetTimestamp BIGINT UNSIGNED
)
BEGIN
    UPDATE DailyReset
    SET
        LastResetTimestamp = InLastResetTimestamp,
        NextResetTimestamp = InNextResetTimestamp
    WHERE CharacterID = InCharacterID;
END;
