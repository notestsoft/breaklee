CREATE PROCEDURE InsertGuild(
    IN InOwnerID INT,
    IN InName VARCHAR(40),
    OUT OutResult TINYINT UNSIGNED,
    OUT OutName VARCHAR(40),
    OUT OutGuildID INT
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    START TRANSACTION;
    
    INSERT INTO Guild (
        OwnerID, 
        Name
    )
    VALUES (
        InOwnerID, 
        InName
    );

    SET OutResult = 0;
    SET OutName = InName;
    SET OutGuildID = LAST_INSERT_ID();

    COMMIT;
END