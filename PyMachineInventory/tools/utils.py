from PySide6.QtCore import QRunnable, Slot
from dataclasses import fields

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
            print(f'Worker Thread Error: {e}')

def dataclass_to_dict(instance:object):
    return { f.name: getattr(instance, f.name) for f in fields(instance) }