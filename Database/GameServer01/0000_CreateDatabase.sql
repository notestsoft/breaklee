CREATE DATABASE GameServer01;

CREATE TABLE GameServer01.Migrations (
    ID INT AUTO_INCREMENT,
    MigrationName VARCHAR(255) NOT NULL UNIQUE,
    AppliedAt BIGINT DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY(ID)
) ENGINE=INNODB DEFAULT CHARSET=utf8;

DELIMITER //

CREATE PROCEDURE GameServer01.ApplyMigration(
    IN InMigrationName VARCHAR(255), 
    IN InMigrationSQL TEXT
)
BEGIN
    IF NOT EXISTS (SELECT 1 FROM Migrations WHERE MigrationName = InMigrationName) THEN
        SET @MigrationQuery = InMigrationSQL;

        PREPARE MigrationStatement FROM @MigrationQuery;
        EXECUTE MigrationStatement;
        DEALLOCATE PREPARE MigrationStatement;

        INSERT INTO Migrations (MigrationName) VALUES (InMigrationName);
    ELSE
        SELECT CONCAT('Migration ', InMigrationName, ' has already been applied.') AS Message;
    END IF;
END //

DELIMITER ;