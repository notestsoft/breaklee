CREATE PROCEDURE Authenticate (
    IN InAccountID INT,
    OUT OutSuccess BOOLEAN,
    OUT OutCharacterSlotID INT,
    OUT OutCharacterSlotOrder BIGINT UNSIGNED,
    OUT OutCharacterSlotOpenMask INT UNSIGNED,
    OUT OutIsSubpasswordSet BOOLEAN,
    OUT OutIsPremium INT,
    OUT OutForceGems INT
)
BEGIN
    DECLARE TempSubpassword VARCHAR(11);

    -- Initialize OutSuccess to FALSE (0)
    SET OutSuccess = 0;

    -- Start the transaction
    START TRANSACTION;

    -- Attempt to select the account details
    SELECT 
        CharacterSlotID,
        CharacterSlotOrder,
        CharacterSlotOpenMask,
        ForceGems
    INTO 
        OutCharacterSlotID,
        OutCharacterSlotOrder,
        OutCharacterSlotOpenMask,
        OutForceGems
    FROM 
        Accounts 
    WHERE 
        AccountID = InAccountID;

    -- Check if any rows were returned
    IF OutCharacterSlotID IS NULL THEN
        -- No account found, so insert a new one with default values
        INSERT INTO Accounts (AccountID) VALUES (InAccountID);
        INSERT INTO GoldMeritMastery (AccountID, MasterySlotData) VALUES (InAccountID, '');
        INSERT INTO PlatinumMeritMastery (AccountID, ExtendedMemorizeSlotData, UnlockedSlotData, MasterySlotData, SpecialMasterySlotData) VALUES (InAccountID, '', '', '', '');
        INSERT INTO DiamondMeritMastery (AccountID, ExtendedMemorizeSlotData, UnlockedSlotData, MasterySlotData, SpecialMasterySlotData) VALUES (InAccountID, '', '', '', '');
        INSERT INTO Collection (AccountID, SlotData) VALUES (InAccountID, '');
        INSERT INTO ResearchSupport (AccountID, MaterialSlotData, ActiveMissionBoard, MissionBoardData) VALUES (InAccountID, '', '', '');
        INSERT INTO EventPass (AccountID, MissionPageData, MissionSlotData, RewardSlotData) VALUES (InAccountID, '', '', '');
        INSERT INTO CostumeWarehouse (AccountID, SlotData) VALUES (InAccountID, '');
        INSERT INTO Warehouse (AccountID, SlotData) VALUES (InAccountID, '');
        INSERT INTO AnimaMastery (AccountID, PresetCount, PresetData, CategoryData) VALUES (InAccountID, 3, x'0001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809', '');
        INSERT INTO Settings (AccountID, HotKeysData, OptionsData, MacrosData) VALUES (InAccountID, '', '', '');

        -- Re-select the newly inserted account details
        SELECT 
            CharacterSlotID,
            CharacterSlotOrder,
            CharacterSlotOpenMask,
            ForceGems
        INTO 
            OutCharacterSlotID,
            OutCharacterSlotOrder,
            OutCharacterSlotOpenMask,
            OutForceGems
        FROM 
            Accounts 
        WHERE 
            AccountID = InAccountID;

        -- Check if the new account details were successfully retrieved
        IF OutCharacterSlotID IS NULL THEN
            SET OutSuccess = 0;
        ELSE
            SET OutSuccess = 1;
        END IF;
    ELSE
        -- Account already exists
        SET OutSuccess = 1;
    END IF;

    -- Check if the account has an active premium service
    IF OutSuccess THEN
        SELECT 
            COUNT(*) INTO @PremiumCount
        FROM 
            Services
        WHERE 
            AccountID = InAccountID
            AND StartedAt <= UNIX_TIMESTAMP()
            AND ExpiredAt >= UNIX_TIMESTAMP();

        -- Set OutIsPremium to TRUE (1) if there are any active premium services, otherwise FALSE (0)
        SET OutIsPremium = (IF(@PremiumCount > 0, 1, 0));
    END IF;

    SET OutIsSubpasswordSet = 0;
    
    -- Check if a subpassword is set
    IF OutSuccess THEN
        SELECT Password
        INTO TempSubpassword
        FROM Subpasswords
        WHERE AccountID = InAccountID AND Type = 1
        LIMIT 1;

        IF TempSubpassword IS NOT NULL THEN
            SET OutIsSubpasswordSet = (CHAR_LENGTH(TempSubpassword) > 0);
        END IF;
    END IF;

    -- Commit or rollback based on success
    IF OutSuccess THEN
        COMMIT;
    ELSE
        ROLLBACK;
    END IF;
END;
