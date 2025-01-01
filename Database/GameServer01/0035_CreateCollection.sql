CREATE TABLE Collection (
    AccountID INT NOT NULL,
    SlotCount SMALLINT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (AccountID),
    
    CONSTRAINT FK_Accounts_Collection FOREIGN KEY (AccountID) 
        REFERENCES Accounts(AccountID) 
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
