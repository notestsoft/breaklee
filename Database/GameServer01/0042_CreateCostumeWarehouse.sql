CREATE TABLE CostumeWarehouse (
    AccountID INT NOT NULL,
    SlotCount INT NOT NULL DEFAULT 0,             
    SlotData BLOB NOT NULL, 
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_CostumeWarehouse FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
