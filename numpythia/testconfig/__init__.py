import os
from pkg_resources import resource_filename


__all__ = [
    'get_config',
]


def get_config(filename):
    return resource_filename('numpythia', os.path.join('testconfig', filename))
