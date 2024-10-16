CREATE PROCEDURE HashPassword(
    IN InPassword VARCHAR(255),
    IN InSalt VARBINARY(16),
    IN InIterations INT,
    OUT OutHash VARBINARY(80)
)
BEGIN
    DECLARE TempHash VARBINARY(64); 
    
    SET TempHash = UNHEX(SHA2(CONCAT(InSalt, InPassword), 512));

    WHILE InIterations > 0 DO
        SET TempHash = UNHEX(SHA2(TempHash, 512)); 
        SET InIterations = InIterations - 1;
    END WHILE;

    SET OutHash = CONCAT(InSalt, TempHash);
END;