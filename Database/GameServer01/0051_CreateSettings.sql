CREATE TABLE Settings (
    AccountID INT NOT NULL,
    HotKeysDataLength INT UNSIGNED NOT NULL DEFAULT 0,
    OptionsDataLength INT UNSIGNED NOT NULL DEFAULT 0,
    MacrosDataLength INT UNSIGNED NOT NULL DEFAULT 0,
    HotKeysData BLOB NOT NULL,                   
    OptionsData BLOB NOT NULL,             
    MacrosData BLOB NOT NULL, 
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_Settings FOREIGN KEY (AccountID)
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
