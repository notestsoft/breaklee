CREATE PROCEDURE SyncHelp (
    IN InCharacterID INT,
    IN InHelpWindow INT UNSIGNED
)
BEGIN
    UPDATE Help
    SET
        HelpWindow = InHelpWindow
    WHERE CharacterID = InCharacterID;
END;
