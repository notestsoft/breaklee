CREATE PROCEDURE SyncResearchSupport (
    IN InAccountID INT,
    IN InExp INT,
    IN InDecodedCircuitCount INT,
    IN InResetCount TINYINT UNSIGNED,
    IN InSeasonStartDate BIGINT UNSIGNED,
    IN InSeasonEndDate BIGINT UNSIGNED,
    IN InMaterialSlotData BLOB,
    IN InActiveMissionBoard BLOB,
    IN InMissionBoardData BLOB
)
BEGIN
    UPDATE ResearchSupport
    SET
        Exp = InExp,
        DecodedCircuitCount = InDecodedCircuitCount,
        ResetCount = InResetCount,
        SeasonStartDate = InSeasonStartDate,
        SeasonEndDate = InSeasonEndDate,
        MaterialSlotData = InMaterialSlotData,
        ActiveMissionBoard = InActiveMissionBoard,
        MissionBoardData = InMissionBoardData
    WHERE AccountID = InAccountID;
END;
