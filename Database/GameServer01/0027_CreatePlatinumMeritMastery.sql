CREATE TABLE PlatinumMeritMastery (
    AccountID INT NOT NULL,
    IsEnabled TINYINT UNSIGNED NOT NULL DEFAULT 0,
    Exp INT NOT NULL DEFAULT 0,
    Points INT NOT NULL DEFAULT 0,
    ActiveMemorizeIndex TINYINT UNSIGNED NOT NULL DEFAULT 0,
    OpenSlotMasteryIndex INT NOT NULL DEFAULT 0,
    OpenSlotUnlockTime BIGINT UNSIGNED NOT NULL DEFAULT 0,
    TotalMemorizeSlotCount SMALLINT NOT NULL DEFAULT 0,
    UnlockedSlotCount SMALLINT NOT NULL DEFAULT 0,
    MasterySlotCount SMALLINT NOT NULL DEFAULT 0,
    SpecialMasterySlotCount SMALLINT NOT NULL DEFAULT 0,
    ExtendedMemorizeSlotCount TINYINT NOT NULL DEFAULT 0,
    ExtendedMemorizeSlotData BLOB NOT NULL,
    UnlockedSlotData BLOB NOT NULL,
    MasterySlotData BLOB NOT NULL,
    SpecialMasterySlotData BLOB NOT NULL,

    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_PlatinumMeritMastery FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
