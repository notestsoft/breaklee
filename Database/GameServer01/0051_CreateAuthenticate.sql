CREATE PROCEDURE Authenticate (
    IN InAccountID INT,
    OUT OutSuccess BOOLEAN,
    OUT OutCharacterSlotID INT,
    OUT OutCharacterSlotOrder BIGINT UNSIGNED,
    OUT OutCharacterSlotOpenMask INT UNSIGNED,
    OUT OutIsSubpasswordSet BOOLEAN,
    OUT OutIsPremium BOOLEAN,
    OUT OutForceGems INT
)
BEGIN
    DECLARE Success BOOLEAN DEFAULT TRUE;

    -- Error handling: Declare handlers for specific errors
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        -- Rollback on any SQL exception
        ROLLBACK;
        SET OutSuccess = FALSE;
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'SQL Exception: An unexpected error occurred';
    END;

    DECLARE EXIT HANDLER FOR NOT FOUND
    BEGIN
        -- Handle specific case where no account is found after insertion attempt
        ROLLBACK;
        SET OutSuccess = FALSE;
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Account could not be found or created';
    END;

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
    IF ROW_COUNT() = 0 THEN
        -- No account found, so insert a new one with default values
        INSERT INTO Accounts (AccountID) 
        VALUES (InAccountID);

        -- Check if the insert was successful
        IF ROW_COUNT() != 1 THEN
            SET Success = FALSE;
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert new account in Accounts table';
        END IF;

        -- Perform all other inserts as part of the same transaction
        IF Success THEN
            INSERT INTO GoldMeritMastery (AccountID, MasterySlotData)
            VALUES (InAccountID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert GoldMeritMastery data';
            END IF;
        END IF;

        -- Repeat similar checks for other inserts
        IF Success THEN
            INSERT INTO PlatinumMeritMastery (AccountID, ExtendedMemorizeSlotData, UnlockedSlotData, MasterySlotData, SpecialMasterySlotData)
            VALUES (InAccountID, '', '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert PlatinumMeritMastery data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO DiamondMeritMastery (AccountID, ExtendedMemorizeSlotData, UnlockedSlotData, MasterySlotData, SpecialMasterySlotData)
            VALUES (InAccountID, '', '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert DiamondMeritMastery data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Collection (AccountID, SlotData)
            VALUES (InAccountID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert Collection data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO ResearchSupport (AccountID, MaterialSlotData, ActiveMissionBoard, MissionBoardData)
            VALUES (InAccountID, '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert ResearchSupport data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO EventPass (AccountID, MissionSlotData, RewardSlotData)
            VALUES (InAccountID, '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert EventPass data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO CostumeWarehouse (AccountID, SlotData)
            VALUES (InAccountID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert CostumeWarehouse data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Warehouse (AccountID, SlotData)
            VALUES (InAccountID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert Warehouse data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO AnimaMastery (AccountID, PresetData, CategoryData)
            VALUES (InAccountID, '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert AnimaMastery data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Settings (AccountID, HotKeysData, OptionsData, MacrosData)
            VALUES (InAccountID, '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert Settings data';
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Subpasswords (AccountID) 
            VALUES (InAccountID);

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to insert Subpassword data';
            END IF;
        END IF;

        -- Re-select the newly inserted account details
        IF Success THEN
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
            IF ROW_COUNT() = 0 THEN
                SET Success = FALSE;
                SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Error: Failed to retrieve new account details';
            END IF;
        END IF;
        
        -- Commit or rollback based on success
        IF Success THEN
            COMMIT;
            SET OutSuccess = TRUE;
        ELSE
            ROLLBACK;
            SET OutSuccess = FALSE;
        END IF;
    ELSE
        -- Account already exists, proceed with additional checks

        -- Check if the account has an active premium service
        SELECT 
            COUNT(*) INTO @PremiumCount
        FROM 
            Services
        WHERE 
            AccountID = InAccountID
            AND StartedAt <= UNIX_TIMESTAMP()
            AND ExpiredAt >= UNIX_TIMESTAMP();

        -- Set OutIsPremium to TRUE if there are any active premium services, otherwise FALSE
        SET OutIsPremium = (@PremiumCount > 0);

        -- Check if a subpassword is set
        SELECT COUNT(*) INTO @SubpasswordCount
        FROM Subpasswords 
        WHERE AccountID = InAccountID;

        SET OutIsSubpasswordSet = (@SubpasswordCount > 0);

        -- Account exists, commit transaction
        COMMIT;
        SET OutSuccess = TRUE;
    END IF;
END;
