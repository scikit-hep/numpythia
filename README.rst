.. -*- mode: rst -*-

numpythia: The interface between PYTHIA and NumPy
=================================================

.. image:: https://img.shields.io/pypi/v/numpythia.svg
   :target: https://pypi.python.org/pypi/numpythia

numpythia provides an interface between `PYTHIA
<http://home.thep.lu.se/~torbjorn/Pythia.html>`_ and `NumPy
<http://www.numpy.org/>`_ allowing you to generate events as NumPy arrays of
particle four-momenta. By default numpythia only depends on NumPy and builds
internal copies of the PYTHIA and `HepMC <http://hepmc.web.cern.ch/hepmc/>`_
source code.

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

Also see `pyjet <https://github.com/ndawe/pyjet>`_ for jet clustering.


Standalone Installation
-----------------------

To simply use the built-in PYTHIA and HepMC::

   pip install --user -v numpythia

And you're good to go!

Support for building against an external PYTHIA, Delphes, and much more will
come soon.
