CREATE PROCEDURE SyncEventPass (
    IN InAccountID INT,
    IN InStartDate BIGINT UNSIGNED,
    IN InEndDate BIGINT UNSIGNED,
    IN InMissionPageCount INT,
    IN InMissionSlotCount INT,
    IN InRewardSlotCount INT,
    IN InMissionPageData BLOB,
    IN InMissionSlotData BLOB,
    IN InRewardSlotData BLOB
)
BEGIN
    UPDATE EventPass
    SET
        StartDate = InStartDate,
        EndDate = InEndDate,
        MissionPageCount = InMissionPageCount,
        MissionSlotCount = InMissionSlotCount,
        RewardSlotCount = InRewardSlotCount,
        MissionPageData = InMissionPageData,
        MissionSlotData = InMissionSlotData,
        RewardSlotData = InRewardSlotData
    WHERE AccountID = InAccountID;
END;
