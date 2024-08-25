CREATE TABLE EventPass (
    AccountID INT NOT NULL,
    StartDate BIGINT UNSIGNED NOT NULL DEFAULT 0,
    EndDate BIGINT UNSIGNED NOT NULL DEFAULT 0,
    MissionPageCount INT NOT NULL DEFAULT 0,
    MissionSlotCount INT NOT NULL DEFAULT 0,
    RewardSlotCount INT NOT NULL DEFAULT 0,
    MissionPageData BLOB NOT NULL,
    MissionSlotData BLOB NOT NULL,
    RewardSlotData BLOB NOT NULL,
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_EventPass FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
