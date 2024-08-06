#ifndef STATEMENT
#define STATEMENT(__NAME__, __QUERY__)
#endif

STATEMENT(
    MASTERDB_INSERT_ACCOUNT, 
    "INSERT INTO `accounts` (`account_id`) VALUES (?);"
)

STATEMENT(
    MASTERDB_SELECT_ACCOUNT_BY_ID, 
    "SELECT `account_id`, `session_ip`, `session_timeout`, `character_password`, `character_question`, `character_answer`, `created_at`, `updated_at`, `AccountInfo`.`data` AS `AccountInfo_data` "
    "FROM `accounts` "
    "LEFT JOIN `AccountAccountInfo` AS `AccountInfo` ON `AccountInfo`.`AccountID` = `accounts`.`account_id` "
    "WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_SESSION,
    "UPDATE `accounts` SET `session_ip` = ?, `session_timeout` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_ACCOUNT_CHARACTER_PASSWORD,
    "UPDATE `accounts` SET `character_password` = ?, `character_question` = ?, `character_answer` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_INSERT_CHARACTER,
    "INSERT INTO `characters` (`account_id`, `name`, `index`) VALUES (?, ?, ?);"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_BY_ID,
    "SELECT `account_id`, `id`, `name`, `index`, `created_at`, `updated_at` FROM `characters` WHERE `id` = ?;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_BY_ACCOUNT,
    "SELECT `account_id`, `id`, `name`, `index`, `created_at`, `updated_at` FROM `characters` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_INDEX_BY_ACCOUNT,
    "SELECT `characters`.`account_id`, `characters`.`id`, `characters`.`name`, `characters`.`index`, `info`.`data` AS `info_data`, `equipment`.`data` AS `equipment_data`, `overlordmastery`.`data` AS `overlordmastery_data`, `characters`.`created_at`, `characters`.`updated_at` "
    "FROM `characters` "
    "LEFT JOIN `CharacterInfo` AS `info` ON `info`.`CharacterID` = `characters`.`id` "
    "LEFT JOIN `CharacterEquipmentInfo` AS `equipment` ON `equipment`.`CharacterID` = `characters`.`id` "
    "LEFT JOIN `CharacterOverlordMasteryInfo` AS `overlordmastery` ON `overlordmastery`.`CharacterID` = `characters`.`id` "
    "WHERE `characters`.`account_id` = ? ;"
)

STATEMENT(
    MASTERDB_SELECT_CHARACTER_NAME_COUNT,
    "SELECT COUNT(*) FROM `characters` WHERE `name` = ?;"
)

STATEMENT(
    MASTERDB_DELETE_CHARACTER,
    "DELETE FROM `characters` WHERE `account_id` = ? AND `id` = ?;"
)

STATEMENT(
    MASTERBD_INSERT_SUBPASSWORD,
    "INSERT INTO `subpasswords` (`account_id`) VALUES (?);"
)

STATEMENT(
    MASTERBD_SELECT_SUBPASSWORD,
    "SELECT `account_id`, `equipment_password`, `equipment_question`, `equipment_answer`, `equipment_locked`, `warehouse_password`, `warehouse_question`, `warehouse_answer`, `warehouse_locked`, `created_at`, `updated_at` FROM `subpasswords` WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_SUBPASSWORD_EQUIPMENT,
    "UPDATE `subpasswords` SET `equipment_password` = ?, `equipment_question` = ?, `equipment_answer` = ?, `equipment_locked` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_UPDATE_SUBPASSWORD_WAREHOUSE,
    "UPDATE `subpasswords` SET `warehouse_password` = ?, `warehouse_question` = ?, `warehouse_answer` = ?, `warehouse_locked` = ? WHERE `account_id` = ?;"
)

STATEMENT(
    MASTERDB_INSERT_SERVICE,
    "INSERT INTO `services` (`account_id`, `service_type`, `started_at`, `expired_at`) VALUES (?, ?, UNIX_TIMESTAMP(), UNIX_TIMESTAMP() + ?);"
)

STATEMENT(
    MASTERDB_SELECT_CURRENT_ACTIVE_SERVICE_BY_ACCOUNT,
    "SELECT `id`, `account_id`, `service_type`, `started_at`, `expired_at`, `created_at`, `updated_at` FROM `services` WHERE `started_at` <= UNIX_TIMESTAMP() AND `expired_at` >= UNIX_TIMESTAMP() AND `account_id` = ?;"
)

#undef STATEMENT
