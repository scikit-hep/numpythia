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

   from numpythia import generate, STATUS, HAS_END_VERTEX, ABS_PDG_ID
   from numpythia.testcmnd import get_cmnd

   for event in generate(
         get_cmnd('w'), events=1,
         find=(STATUS == 1) & ~HAS_END_VERTEX &
              (ABS_PDG_ID != 12) & (ABS_PDG_ID != 14) & (ABS_PDG_ID != 16),
         random_state=1):
      print(event)

The dtype of each generated event is:

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
