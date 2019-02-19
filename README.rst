.. -*- mode: rst -*-

numpythia: The interface between PYTHIA and NumPy
=================================================

.. image:: https://img.shields.io/pypi/v/numpythia.svg
   :target: https://pypi.python.org/pypi/numpythia

.. image:: https://zenodo.org/badge/DOI/10.5281/zenodo.1471492.svg
  :target: https://doi.org/10.5281/zenodo.1471492

.. image:: https://travis-ci.org/scikit-hep/numpythia.svg?branch=master
   :target: https://travis-ci.org/scikit-hep/numpythia

.. image:: https://coveralls.io/repos/github/scikit-hep/numpythia/badge.svg?branch=master
   :target: https://coveralls.io/github/scikit-hep/numpythia?branch=master

numpythia provides an interface between `PYTHIA
<http://home.thep.lu.se/Pythia/>`_ and `NumPy
<http://www.numpy.org/>`_ allowing you to generate events as NumPy arrays of
particle four-momenta. By default numpythia only depends on NumPy and builds
internal copies of the PYTHIA and `HepMC <http://hepmc.web.cern.ch/hepmc/>`_
source code.

Standalone Installation
-----------------------

To simply use the built-in PYTHIA and HepMC::

   pip install -v numpythia

And you're good to go!

Support for building against an external PYTHIA is on the wishlist.

Strict dependencies
-------------------

- `Python <http://docs.python-guide.org/en/latest/starting/installation/>`__ (2.7+, 3.4+)
- `Numpy <https://scipy.org/install.html>`__
- `six <https://six.readthedocs.io/>`__

Getting started
---------------

.. code-block:: python

   from numpythia import Pythia, hepmc_write, hepmc_read
   from numpythia import STATUS, HAS_END_VERTEX, ABS_PDG_ID
   from numpythia.testcmnd import get_cmnd
   from numpy.testing import assert_array_equal

   pythia = Pythia(get_cmnd('w'), random_state=1)

   selection = ((STATUS == 1) & ~HAS_END_VERTEX &
               (ABS_PDG_ID != 12) & (ABS_PDG_ID != 14) & (ABS_PDG_ID != 16))

   # generate events while writing to ascii hepmc
   for event in hepmc_write('events.hepmc', pythia(events=1)):
      array1 = event.all(selection)

   # read the same event back from ascii hepmc
   for event in hepmc_read('events.hepmc'):
      array2 = event.all(selection)

   assert_array_equal(array1, array2)

The dtype of any array of particle information is:

.. code-block:: python

   np.dtype([('E', 'f8'), ('px', 'f8'), ('py', 'f8'), ('pz', 'f8'), ('mass', 'f8'),
             ('prodx', 'f8'), ('prody', 'f8'), ('prodz', 'f8'), ('prodt', 'f8'),
             ('pdgid', 'i4'), ('status', 'i4')])

Also see `pyjet <https://github.com/scikit-hep/pyjet>`_ for jet clustering.

Tutorial
--------

Setting PYTHIA
~~~~~~~~~~~~~~

PYTHIA settings can be passed in one of three ways: through the `**kwargs` arguments of the constructor `Pythia(..., **kwargs)`:

.. code-block:: python

   pythia = Pythia(..., Beams_eCM=13000.)

Or as a dictionary:

.. code-block:: python

   pythia = Pythia(..., params={'Beams:eCM':  13000.})

Or via a Python command file:

.. code-block:: python

   pythia = Pythia(config='path/to/config.cmd')

The full list of settings can be found on the  `PYTHIA homepage <http://home.thep.lu.se/Pythia/>`_.

Note that the ":" in settings names is replaced by a "_"  if using `kwargs`.
`kwargs` take precedence over `params` and they both take precedence over `config`.
Example config files can be found under the `numpythia.testcmnd` directory.

Generate events
~~~~~~~~~~~~~~~

To generate events do

.. code-block:: python

   events = pythia(events=100)

where events is generator of ``GenEvent``.
