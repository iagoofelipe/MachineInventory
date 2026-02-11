-- database: ../machine.db

CREATE TABLE user (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    cpf TEXT NOT NULL UNIQUE
);

CREATE TABLE machine (
    id TEXT PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    os TEXT NOT NULL,
    osArchitecture TEXT,
    osInstallDate DATETIME,
    osVersion TEXT,
    osOrganization TEXT,
    osSerialNumber TEXT,
    motherboardManufacturer TEXT,
    processor TEXT,
    configDate DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    userId TEXT NOT NULL,
    FOREIGN KEY(userId) REFERENCES user(id)
);

CREATE TABLE program (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT NOT NULL,
    version TEXT,
    installDate DATETIME,
    estimatedSize INTEGER,
    publisher TEXT,
    currentUserOnly BOOLEAN
);

CREATE TABLE machine_program (
    id TEXT PRIMARY KEY NOT NULL,
    programId TEXT NOT NULL,
    machineId TEXT NOT NULL,
    FOREIGN KEY(programId) REFERENCES program(id),
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE machine_disk (
    id TEXT PRIMARY KEY NOT NULL,
    freeSpace INTEGER,
    size INTEGER,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE machine_network_adapter (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT,
    mac TEXT,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);

CREATE TABLE machine_physical_memory (
    id TEXT PRIMARY KEY NOT NULL,
    name TEXT,
    capacity INTEGER,
    speed INTEGER,
    machineId TEXT NOT NULL,
    FOREIGN KEY(machineId) REFERENCES machine(id)
);