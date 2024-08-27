CREATE TABLE AnimaMastery (
    AccountID INT NOT NULL,
    PresetCount INT NOT NULL, 
    StorageCount INT NOT NULL DEFAULT 0, 
    UnlockedCategoryFlags INT UNSIGNED NOT NULL DEFAULT 0, 
    PresetData BLOB NOT NULL,
    CategoryData BLOB NOT NULL, 
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_AnimaMastery FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;