-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               11.4.4-MariaDB - mariadb.org binary distribution
-- Server OS:                    Win64
-- HeidiSQL Version:             12.10.0.7000
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

-- Dumping structure for procedure gameserver01.GetCharacterIDByName
DELIMITER //
CREATE PROCEDURE `GetCharacterIDByName`(
	IN `CharacterName` VARCHAR(50)
)
BEGIN
	SELECT CharacterID FROM characters WHERE NAME=TRIM(CharacterName) LIMIT 1;
END//
DELIMITER ;

-- Dumping structure for procedure gameserver01.GetCharacterViewEqData
DELIMITER //
CREATE PROCEDURE `GetCharacterViewEqData`(
	IN `InCharacterID` INT
)
BEGIN
    SELECT 
    	  characters.Name AS 'Name',
        Characters.Level AS Level,
    	  CASE
        WHEN (Style & 0x0F) >= 10 THEN (Style & 0x0F) - 8
        ELSE (Style & 0x0F)
    	  END AS Style,
        Settings.OptionsDataLength,
        Equipment.EquipmentSlotCount,
        Equipment.EquipmentSlotData,
        Settings.OptionsData
    FROM Characters
    LEFT JOIN Equipment ON Characters.CharacterID = Equipment.CharacterID
    LEFT JOIN Settings ON Characters.AccountID = Settings.AccountID
    WHERE Characters.CharacterID = InCharacterID;
END//
DELIMITER ;

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
