CREATE TABLE ResearchSupport (
    AccountID INT NOT NULL,
    Exp INT NOT NULL DEFAULT 0,
    DecodedCircuitCount INT NOT NULL DEFAULT 0,
    ResetCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    SeasonStartDate BIGINT UNSIGNED NOT NULL DEFAULT 0,
    SeasonEndDate BIGINT UNSIGNED NOT NULL DEFAULT 0,
    MaterialSlotData BLOB NOT NULL, 
    ActiveMissionBoard BLOB NOT NULL, 
    MissionBoardData BLOB NOT NULL, 
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_ResearchSupport FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
