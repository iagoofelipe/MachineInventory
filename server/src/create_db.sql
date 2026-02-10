-- database: ../machine.db

CREATE TABLE machine (
    id TEXT PRIMARY KEY NOT NULL,
    cpfOwner TEXT NOT NULL,
    osName TEXT NOT NULL,
    osArchitecture TEXT,
    osInstallDate DATETIME,
    osVersion TEXT,
    osOrganization TEXT,
    osSerialNumber TEXT,
    motherboardManufacturer TEXT,
    processor TEXT,
    configDate DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE program (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    installDate DATETIME,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE disk (
    id TEXT PRIMARY KEY NOT NULL,
    freeSpace INTEGER,
    size INTEGER,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE network_adapter (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT,
    mac TEXT,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE physical_memory (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT,
    capacity INTEGER,
    speed INTEGER,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);