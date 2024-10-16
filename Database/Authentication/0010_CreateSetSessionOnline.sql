CREATE PROCEDURE SetSessionOnline (
    IN InAccountID INT,
    IN InOnline BOOLEAN
)
BEGIN
    UPDATE Sessions 
    SET 
        Online = InOnline, 
        UpdatedAt = UNIX_TIMESTAMP() 
    WHERE AccountID = InAccountID;
END;