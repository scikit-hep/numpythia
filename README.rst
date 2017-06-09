.. -*- mode: rst -*-

numpythia: The interface between PYTHIA and NumPy
=================================================

numpythia provides an interface between `PYTHIA
<http://home.thep.lu.se/~torbjorn/Pythia.html>`_ and `NumPy
<http://www.numpy.org/>`_ allowing you to generate events as NumPy arrays of
particle four-momenta. By default numpythia only depends on NumPy and builds
internal copies of the PYTHIA and `HepMC <http://hepmc.web.cern.ch/hepmc/>`_
source code.

.. code-block:: python

   from numpythia import generate
   from numpythia.testcmnd import get_cmnd

   for event in generate(get_cmnd('w.cmnd'), events=10):
       print(event)


Standalone Installation
-----------------------

To simply use the built-in PYTHIA and HepMC::

   pip install --user -v numpythia

And you're good to go!

Support for building against an external PYTHIA, Delphes, and much more will
come soon.
