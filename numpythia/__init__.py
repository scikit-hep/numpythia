from ._libnumpythia import generate_events as _generate_events
from ._libnumpythia import PythiaInput, HepMCInput
import os
from fnmatch import fnmatch
import logging
from .extern.six import string_types

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
        xmldoc = os.environ.get('PYTHIA8DATA', os.path.join(
            os.environ.get('DEEPJETS_SFT_DIR', '/usr/local'),
            'share/Pythia8/xmldoc'))
        if not os.path.exists(filename):
            internal_filename = os.path.join(
                os.environ.get('DEEPJETS_DIR'), 'config', 'pythia', filename)
            if not os.path.isabs(filename) and os.path.exists(internal_filename):
                log.warning("{0} does not exist but using internal "
                            "config with the same name instead: {1}".format(
                                filename, internal_filename))
                filename = internal_filename
            else:
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


def generate(gen_input, events=-1, write_to='', ignore_weights=False, **kwargs):
    if isinstance(gen_input, string_types):
        if fnmatch(os.path.splitext(gen_input)[1], '.hepmc*'):
            gen_input = get_input('hepmc', gen_input, **kwargs)
        else:
            gen_input = get_input('pythia', gen_input, **kwargs)
    for event in _generate_events(gen_input, events, write_to, ignore_weights):
        yield event
