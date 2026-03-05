-- database: ./database.db
CREATE TABLE IF NOT EXISTS `machine_version` (
    id TEXT PRIMARY KEY NOT NULL,
    datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    mac TEXT NOT NULL,
    previous_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `version_remove` (
    id TEXT PRIMARY KEY NOT NULL,
    table_name TEXT NOT NULL,
    ref_id TEXT NOT NULL,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine` (
    id TEXT PRIMARY KEY NOT NULL,
    os TEXT NOT NULL,
    title TEXT NOT NULL,
    osArchitecture TEXT,
    osInstallDate TEXT,
    osVersion TEXT,
    osSerialNumber TEXT,
    organization TEXT,
    motherboard TEXT,
    motherboardManufacturer TEXT,
    processor TEXT,
    processorClockSpeed TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `disk` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    serialNumber TEXT,
    model TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `network_adapter` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    mac TEXT NOT NULL,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `physical_memory` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    capacity TEXT NOT NULL,
    speed TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `program` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    publisher TEXT,
    version TEXT,
    estimatedSize TEXT,
    currentUserOnly INTEGER NOT NULL DEFAULT 0,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);