from ._libnumpythia import _Pythia as Pythia, ReaderAscii, WriterAscii
from ._libnumpythia import FILTERS, ALL, FIRST, LAST
from ._libnumpythia import ANCESTORS, DESCENDANTS, PARENTS, CHILDREN, PRODUCTION_SIBLINGS
import logging

locals().update(FILTERS)

log = logging.getLogger(__name__)

__all__ = [
    'Pythia',
    'hepmc_read',
    'hepmc_write',
]


def hepmc_read(filename):
    reader = ReaderAscii(filename)
    for event in reader:
        yield event

def hepmc_write(filename, source):
    writer = WriterAscii(filename)
    for event in source:
        writer.write(event)
        yield event
