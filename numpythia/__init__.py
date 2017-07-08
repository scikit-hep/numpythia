from ._libnumpythia import _Pythia as Pythia, ReaderAscii, WriterAscii
from ._libnumpythia import FILTERS
import logging

locals().update(FILTERS)

log = logging.getLogger(__name__)

__all__ = [
    'Pythia',
    'hepmc_read',
    'hepmc_write',
]

PYTHIA_VERSION = '8.226'
HEPMC_VERSION = '3.0.0'


def hepmc_read(filename):
    reader = ReaderAscii(filename)
    for event in reader:
        yield event

def hepmc_write(filename, source):
    writer = WriterAscii(filename)
    for event in source:
        writer.write(event)
        yield event
