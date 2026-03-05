-- database: ./database.db
CREATE TABLE IF NOT EXISTS `machine_version` (
    id TEXT PRIMARY KEY NOT NULL,
    datetime DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    mac TEXT NOT NULL,
    previous_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine_remove` (
    id TEXT PRIMARY KEY NOT NULL,
    table TEXT NOT NULL,
    ref_id TEXT NOT NULL,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine` (
    id TEXT PRIMARY KEY NOT NULL,
    os TEXT NOT NULL,
    title TEXT NOT NULL,
    os_architecture TEXT,
    os_install_date TEXT,
    os_version TEXT,
    os_serial_number TEXT,
    organization TEXT,
    motherboard TEXT,
    motherboard_manufacturer TEXT,
    processor TEXT,
    processor_clock_speed TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine_disk` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    serial_number TEXT,
    model TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine_network_adapter` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    mac TEXT NOT NULL,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine_physical_memory` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    capacity TEXT NOT NULL,
    speed TEXT,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);

CREATE TABLE IF NOT EXISTS `machine_program` (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    publisher TEXT,
    version TEXT,
    estimated_size TEXT,
    current_user_only INTEGER NOT NULL DEFAULT 0,
    machine_version_id TEXT REFERENCES `machine_version`(id)
);