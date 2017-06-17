from ._libnumpythia import generate as _generate
from ._libnumpythia import PythiaInput, HepMCInput
from ._libnumpythia import FILTERS, ALL, FIRST, LAST
from .extern.six import string_types
from pkg_resources import resource_filename
from fnmatch import fnmatch
import logging
import os

locals().update(FILTERS)

log = logging.getLogger(__name__)

__all__ = [
    'generate', 'get_input',
    'PythiaInput', 'HepMCInput',
]


def get_input(name, filename, **kwargs):
    """
    name may be 'pythia' or 'hepmc'
    filename may be the pythia config file or a HepMC file
    """
    name = name.lower().strip()
    if name == 'pythia':
        xmldoc = resource_filename('numpythia', 'src/pythia8226/share')
        if not os.path.exists(filename):
            raise IOError("Pythia config not found: {0}".format(filename))
        gen_input = PythiaInput(filename, xmldoc, **kwargs)
    elif name == 'hepmc':
        gen_input = HepMCInput(filename)
        if kwargs:
            raise ValueError(
                "unrecognized parameters in kwargs: {0}".format(kwargs))
    else:
        raise ValueError(
            "no generator input available with name '{0}'".format(name))
    return gen_input


def generate(gen_input, events=-1, find=None, select=ALL,
             write_to='', weighted=False, **kwargs):
    if isinstance(gen_input, string_types):
        if fnmatch(os.path.splitext(gen_input)[1], '.hepmc*'):
            gen_input = get_input('hepmc', gen_input, **kwargs)
        else:
            gen_input = get_input('pythia', gen_input, **kwargs)
    for event in _generate(gen_input, events, find, select, write_to, weighted):
        yield event
