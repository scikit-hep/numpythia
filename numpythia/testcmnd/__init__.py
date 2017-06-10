import os
from pkg_resources import resource_filename


__all__ = [
    'get_cmnd',
]


def get_cmnd(name):
    return resource_filename('numpythia',
                             os.path.join('testcmnd', name + '.cmnd'))
