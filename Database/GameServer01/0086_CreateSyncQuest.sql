CREATE PROCEDURE SyncQuest (
    IN InCharacterID INT,
    IN InQuestSlotCount SMALLINT UNSIGNED,
    IN InFinishedQuestData BLOB,
    IN InDeletedQuestData BLOB,
    IN InFinishedQuestDungeonData BLOB,
    IN InFinishedMissionDungeonData BLOB,
    IN InQuestSlotData BLOB
)
BEGIN
    UPDATE Quest
    SET
        QuestSlotCount = InQuestSlotCount,
        FinishedQuestData = InFinishedQuestData,
        DeletedQuestData = InDeletedQuestData,
        FinishedQuestDungeonData = InFinishedQuestDungeonData,
        FinishedMissionDungeonData = InFinishedMissionDungeonData,
        QuestSlotData = InQuestSlotData
    WHERE CharacterID = InCharacterID;
END;
