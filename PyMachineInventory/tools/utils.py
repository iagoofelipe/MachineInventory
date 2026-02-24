from PySide6.QtWidgets import QWidget
from PySide6.QtCore import QRunnable, Slot
from dataclasses import fields, is_dataclass
from typing import Iterable
from models import structs
import wmi
import datetime as dt
import logging as log

class Worker(QRunnable):
    def __init__(self, func, *args, callback=None, use_return=False, **kwargs):
        super().__init__()
        self._func = func
        self._args = args
        self._cb = callback
        self._return = use_return
        self._kwargs = kwargs
    
    @Slot()
    def run(self):
        try:
            r = self._func(*self._args, **self._kwargs)
            if self._cb:
                self._cb(r) if self._return else self._cb()
        except Exception as e:
            log.error(f'Worker Thread Error: {e}')

def dataclass_to_dict(instance:object, recursive=False, **extra):
    d = {}

    for f in fields(instance):
        name = f.name
        val = extra[name] if name in extra else getattr(instance, name)

        if recursive and type(val) is list:
            val = list(map(lambda v: dataclass_to_dict(v) if is_dataclass(v) else v, val))

        d[name] = val
    
    d.update(extra)
    
    return d

def set_unenabled(widgets:Iterable[QWidget], unenabled=True):
    changes = []

    if unenabled:
        for wid in widgets:
            if wid.isEnabled():
                wid.setEnabled(False)
                changes.append(wid)
        
    else:
        for wid in widgets:
            wid.setEnabled(True)
    
    return changes

def get_machine_data():
    wmi_obj = wmi.WMI()
    os = wmi_obj.Win32_OperatingSystem()[0]
    motherboard = wmi_obj.Win32_BaseBoard()[0]
    processor = wmi_obj.Win32_Processor()[0]

    machine = structs.Machine(
        os.Caption,
        os.OSArchitecture,
        dt.datetime.strptime(os.InstallDate.split('.')[0], '%Y%m%d%H%M%S'),
        os.Version,
        os.SerialNumber,
        os.Organization,
        motherboard.Product,
        motherboard.Manufacturer,
        processor.Name.strip() if type(processor.Name) is str else None,
        processor.CurrentClockSpeed,
        [ structs.Disk(x.Caption, x.SerialNumber, x.Size, x.Model) for x in wmi_obj.Win32_DiskDrive() ],
        [ structs.NetworkAdapter(x.Name, x.MACAddress) for x in wmi_obj.Win32_NetworkAdapter() if x.MACAddress ],
        [ structs.PhysicalMemory(x.Tag, x.Capacity, x.Speed) for x in wmi_obj.Win32_PhysicalMemory() ],
        [ ],
    )

    # coletando programas
    HKCU = 0x80000001
    HKLM = 0x80000002
    root_subkey = r'Software\Microsoft\Windows\CurrentVersion\Uninstall'
    reg = wmi_obj.StdRegProv
    values = [ 
        ('name', 'DisplayName'),
        ('version', 'DisplayVersion'),
        ('publisher', 'Publisher'),
        ('estimatedSize', 'EstimatedSize'),
    ]

    for key in (HKLM, HKCU):
        result, subkeys = reg.EnumKey(hDefKey=key, sSubKeyName=root_subkey)

        if result != 0 or subkeys is None:
            log.error(f'não foi possível extrair as subchaves do root {key}')
            continue

        for s in subkeys:
            subkey = fr'{root_subkey}\{s}'
            program = structs.Program('', '', '', 0, key == HKCU)

            for field, value in values:
                getter = reg.GetDWORDValue if value == 'EstimatedSize' else reg.GetStringValue
                default = 0 if value == 'EstimatedSize' else ''
                result, value = getter(hDefKey=key, sSubKeyName=subkey, sValueName=value)
                setattr(program, field, value if result == 0 else default)
            
            if program.name:
                machine.programs.append(program)

    return machine

def get_default_mac(values:list[structs.NetworkAdapter]):
    for v in values:
        name = v.name.lower()
        if 'ethernet' in name or 'realtek' in name:
            return v.mac
    
    # caso não haja nenhuma correspondência, retorna o primeiro
    with_mac = list(filter(lambda v: v.mac, values))
    return with_mac[0].mac if with_mac else ''