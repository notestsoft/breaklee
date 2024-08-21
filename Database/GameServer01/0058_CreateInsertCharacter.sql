CREATE PROCEDURE InsertCharacter (
    IN InAccountID INT,
    IN InName VARCHAR(17),
    IN InSlotIndex TINYINT,
    IN InStyle INT UNSIGNED,
    IN InStatSTR SMALLINT UNSIGNED,
    IN InStatDEX SMALLINT UNSIGNED,
    IN InStatINT SMALLINT UNSIGNED,
    IN InWorldIndex TINYINT UNSIGNED,
    IN InPositionX SMALLINT UNSIGNED,
    IN InPositionY SMALLINT UNSIGNED,
    OUT OutStatus TINYINT,
    OUT OutCharacterID INT
)
BEGIN
    DECLARE CHARACTER_STATUS_DBERROR INT DEFAULT 1;
    DECLARE CHARACTER_STATUS_NAME_OCCUPIED INT DEFAULT 3;
    DECLARE CHARACTER_STATUS_NAME_VALIDATION_FAILED INT DEFAULT 4;
    DECLARE CHARACTER_STATUS_NOT_ALLOWED INT DEFAULT 5;
    DECLARE CHARACTER_STATUS_SUCCESS INT DEFAULT 0xA8;
    DECLARE MIN_NAME_LENGTH INT DEFAULT 4; 
    DECLARE MAX_NAME_LENGTH INT DEFAULT 16;
    
    DECLARE Success BOOLEAN DEFAULT FALSE;
    DECLARE IsNameOccupied INT DEFAULT 0;
    DECLARE IsSlotOpen INT DEFAULT 0;
    DECLARE IsSlotOccupied INT DEFAULT 0;

    SET OutStatus = CHARACTER_STATUS_DBERROR;

    -- Label for the entire procedure
    ProcLabelBody: BEGIN
        -- Check if the character name already exists
        SELECT COUNT(*) INTO IsNameOccupied
        FROM Characters
        WHERE Name = InName;

        IF IsNameOccupied > 0 THEN
            SET OutStatus = CHARACTER_STATUS_NAME_OCCUPIED;
            LEAVE ProcLabelBody;
        END IF;

        -- Validate name length
        IF CHAR_LENGTH(InName) < MIN_NAME_LENGTH THEN
            SET OutStatus = CHARACTER_STATUS_NAME_VALIDATION_FAILED;
            LEAVE ProcLabelBody;
        ELSEIF CHAR_LENGTH(InName) > MAX_NAME_LENGTH THEN
            SET OutStatus = CHARACTER_STATUS_NAME_VALIDATION_FAILED;
            LEAVE ProcLabelBody;
        END IF;
        
        -- Check if the slot is open
        IF EXISTS (
            SELECT 1
            FROM Accounts
            WHERE AccountID = InAccountID
            AND (CharacterSlotOpenMask & POWER(2, InSlotIndex)) > 0
        ) THEN
            SET IsSlotOpen = 1;
        END IF;

        IF IsSlotOpen = 0 THEN
            SET OutStatus = CHARACTER_STATUS_NOT_ALLOWED;
            LEAVE ProcLabelBody;
        END IF;

        -- Check if the slot is occupied
        IF EXISTS (
            SELECT 1
            FROM Characters
            WHERE AccountID = InAccountID
            AND SlotIndex = InSlotIndex
        ) THEN
            SET IsSlotOccupied = 1;
        END IF;

        IF IsSlotOccupied > 0 THEN
            Set OutStatus = CHARACTER_STATUS_NOT_ALLOWED;
            LEAVE ProcLabelBody;
        END IF;
        
        START TRANSACTION;

        INSERT INTO Characters (AccountID, Name, SlotIndex, Style, StatSTR, StatDEX, StatINT, WorldIndex, X, Y) 
        VALUES (InAccountID, InName, InSlotIndex, InStyle, InStatSTR, InStatDEX, InStatINT, InWorldIndex, InPositionX, InPositionY);

        -- Check if the insert was Successful
        IF ROW_COUNT() > 0 THEN
            SET OutCharacterID = LAST_INSERT_ID();
            SET Success = TRUE;
        END IF;

        -- Perform all other inserts as part of the same transaction
        IF Success THEN
            INSERT INTO Equipment (CharacterID, EquipmentSlotData, InventorySlotData, LinkSlotData, LockSlotData) 
            VALUES (OutCharacterID, '', '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Inventory (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO SkillSlot (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO QuickSlot (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Ability (CharacterID, EssenceAbilityData, BlendedAbilityData, KarmaAbilityData) 
            VALUES (OutCharacterID, '', '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO BlessingBead (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO PremiumService (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Quest (
                CharacterID,
                FinishedQuestData, 
                DeletedQuestData, 
                FinishedDungeonData, 
                QuestSlotData
            )
            VALUES (
                OutCharacterID,
                CONCAT(REPEAT(CHAR(0), 512)),
                CONCAT(REPEAT(CHAR(0), 128)),
                CONCAT(REPEAT(CHAR(0), 640)),
                ''
            );
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO DailyQuest (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Mercenary (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Appearance (CharacterID, EquipmentSlotData, InventorySlotData) 
            VALUES (OutCharacterID, '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Achievement (CharacterID, SlotData, RewardSlotData) 
            VALUES (OutCharacterID, '', '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO AchievementExtended (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Craft (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO RequestCraft (
                CharacterID, 
                RegisteredFlagData, 
                FavoriteFlagData, 
                SlotData
            ) 
            VALUES (
                OutCharacterID, 
                CONCAT(REPEAT(CHAR(0), 1024)),
                CONCAT(REPEAT(CHAR(0), 1024)),
                ''
            );
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Buff (CharacterID, BuffSlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Upgrade (CharacterID) 
            VALUES (OutCharacterID);
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO VehicleInventory (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO WarpService (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO OverlordMastery (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO HonorMedal (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
        
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO ForceWing (
                CharacterID, 
                PresetEnabled, 
                PresetTrainingPointCount, 
                TrainingUnlockFlagData,
                ArrivalSkillSlotData,
                ArrivalSkillRestoreSlotData,
                PresetSlotData,
                TrainingSlotData
            ) 
            VALUES (
                OutCharacterID, 
                CONCAT(REPEAT(CHAR(0), 5)),
                CONCAT(REPEAT(CHAR(0), 20)),
                CONCAT(REPEAT(CHAR(0), 12)),
                CONCAT(REPEAT(CHAR(0), 66)),
                CONCAT(REPEAT(CHAR(0), 11)),
                '',
                ''
            );

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Giftbox (CharacterID, SlotData, RewardSlotData) 
            VALUES (OutCharacterID, '', '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Transform (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO SecretShop (
                CharacterID, 
                SlotData
                ) 
            VALUES (
                OutCharacterID, 
                CONCAT(REPEAT(CHAR(0), 30))
            );
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO SkillTranscendence (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO StellarMastery (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO DamageBooster (
                CharacterID, 
                ItemData,
                SlotData
            ) 
            VALUES (
                OutCharacterID, 
                CONCAT(REPEAT(CHAR(0), 8)),
                ''
            );
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO MythMastery (CharacterID, PropertySlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO NewbieSupport (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Costume (CharacterID, PageData, AppliedSlotData) 
            VALUES (OutCharacterID, '', '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Exploration (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO TemporaryInventory (CharacterID, SlotData) 
            VALUES (OutCharacterID, '');

            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Recovery (CharacterID, PriceData, SlotData) 
            VALUES (OutCharacterID, '', '');
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        IF Success THEN
            INSERT INTO Preset (
                CharacterID, 
                ConfigurationData
            ) 
            VALUES (
                OutCharacterID, 
                CONCAT(REPEAT(CHAR(0), 205))
            );
            
            IF ROW_COUNT() != 1 THEN
                SET Success = FALSE;
            END IF;
        END IF;

        -- Commit or rollback based on Success
        IF Success THEN
            COMMIT;
            SET OutStatus = CHARACTER_STATUS_SUCCESS;
        ELSE
            ROLLBACK;
            SET OutStatus = CHARACTER_STATUS_DBERROR;
        END IF;
    
    END ProcLabelBody;
END;
