CREATE TABLE GoldMeritMastery (
    AccountID INT NOT NULL,
    MasterySlotCount INT NOT NULL DEFAULT 0,
    Exp INT NOT NULL DEFAULT 0,
    Points INT NOT NULL DEFAULT 0,
    MasterySlotData BLOB NOT NULL DEFAULT 0x00,

    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_GoldMeritMastery FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;