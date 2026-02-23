from dataclasses import dataclass
import datetime as dt

@dataclass
class Disk:
    name: str
    seriaNumber: str
    size: str
    model: str

@dataclass
class NetworkAdapter:
    name: str
    mac: str

@dataclass
class PhysicalMemory:
    name: str
    capacity: str
    speed: int

@dataclass
class Program:
    name: str
    version: str
    publisher: str
    estimatedSize: int
    currentUserOnly: bool

@dataclass
class Machine:
    os: str
    osArchitecture: str
    osInstallDate: dt.datetime
    osVersion: str
    osSerialNumber: str
    osOrganization: str
    motherboard: str
    motherboardManufacturer: str
    processor: str
    processorClockSpeed: int
    disks:list[Disk]
    networkAdapters:list[NetworkAdapter]
    physicalMemories:list[PhysicalMemory]
    programs:list[Program]