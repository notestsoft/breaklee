CREATE PROCEDURE ClearSessionOnline ()
BEGIN
    UPDATE Sessions 
    SET 
        Online = 0, 
        UpdatedAt = UNIX_TIMESTAMP() 
    WHERE 1=1;
END;