CREATE PROCEDURE SyncQuest (
    IN InCharacterID INT,
    IN InQuestSlotCount SMALLINT UNSIGNED,
    IN InFinishedQuestData BLOB,
    IN InDeletedQuestData BLOB,
    IN InFinishedDungeonData BLOB,
    IN InQuestSlotData BLOB
)
BEGIN
    UPDATE Quest
    SET
        QuestSlotCount = InQuestSlotCount,
        FinishedQuestData = InFinishedQuestData,
        DeletedQuestData = InDeletedQuestData,
        FinishedDungeonData = InFinishedDungeonData,
        QuestSlotData = InQuestSlotData
    WHERE CharacterID = InCharacterID;
END;
