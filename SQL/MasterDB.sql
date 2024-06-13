DROP DATABASE IF EXISTS `master`;

CREATE DATABASE `master`;

USE `master`;

DROP TABLE IF EXISTS `accounts`;

CREATE TABLE `accounts` (
    `account_id` INT NOT NULL,

    `session_ip` VARCHAR(15) NOT NULL DEFAULT '127.0.0.1',
    `session_timeout` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    `character_slot_id` INT NOT NULL DEFAULT 0,
    `character_slot_order` BIGINT UNSIGNED NOT NULL DEFAULT 0x0123456789ABCDEF,
    `character_slot_flags` INT UNSIGNED NOT NULL DEFAULT 0x3F,
    `character_password` VARCHAR(11) DEFAULT NULL,
    `character_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `character_answer` VARCHAR(17) DEFAULT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`account_id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS `subpasswords`;

CREATE TABLE `subpasswords` (
    `account_id` INT NOT NULL,

    `equipment_password` VARCHAR(11) DEFAULT NULL,
    `equipment_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `equipment_answer` VARCHAR(17) DEFAULT NULL,
    `equipment_locked` BOOLEAN DEFAULT FALSE,

    `warehouse_password` VARCHAR(11) DEFAULT NULL,
    `warehouse_question` INT UNSIGNED NOT NULL DEFAULT 0,
    `warehouse_answer` VARCHAR(17) DEFAULT NULL,
    `warehouse_locked` BOOLEAN DEFAULT FALSE,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`account_id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS `services`;

CREATE TABLE `services` (
    `id` INT NOT NULL AUTO_INCREMENT,

    `account_id` INT NOT NULL,

    `service_type` INT UNSIGNED NOT NULL,
    `started_at` BIGINT UNSIGNED NOT NULL,
    `expired_at` BIGINT UNSIGNED NOT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`id`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;

DROP TABLE IF EXISTS `characters`;

CREATE TABLE `characters` (
    `account_id` INT NOT NULL,
    
    `id` INT NOT NULL AUTO_INCREMENT,
    `name` VARCHAR(16) NOT NULL,
    `index` TINYINT UNSIGNED NOT NULL,

    `created_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),
    `updated_at` BIGINT UNSIGNED NOT NULL DEFAULT UNIX_TIMESTAMP(),

    PRIMARY KEY (`id`),
    UNIQUE (`account_id`, `index`),
    UNIQUE (`name`)
) ENGINE = INNODB DEFAULT CHARSET = utf8;
