.. -*- mode: rst -*-

``numpythia`` has not been actively maintained for a couple of years. This is mostly due to the emergence of new alternatives which are both faster and more flexible.
For example:

* The `official Pythia 8 (SWIG) bindings available from Conda. <https://anaconda.org/conda-forge/pythia8>`_
* The `impy <https://github.com/impy-project/impy>`_ project implementing a generic user interface to popular event generators
  used in cosmic ray and high-energy particle physics.

numpythia: The interface between PYTHIA and NumPy
=================================================

.. image:: https://img.shields.io/pypi/v/numpythia.svg
   :target: https://pypi.python.org/pypi/numpythia

.. image:: https://zenodo.org/badge/DOI/10.5281/zenodo.1471492.svg
  :target: https://doi.org/10.5281/zenodo.1471492

.. image:: https://github.com/scikit-hep/numpythia/workflows/Main/badge.svg?branch=master
   :target: https://github.com/scikit-hep/numpythia/actions
   :alt: Test status

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

Note that if you are using a Mac OSX system, then installation may require setting an
environment variable as `explained here <https://github.com/pytorch/pytorch/issues/1434>`_.

Strict dependencies
-------------------

- `Python <http://docs.python-guide.org/en/latest/starting/installation/>`__ (2.7+, 3.7+)
- `Numpy <https://scipy.org/install.html>`__
- `six <https://six.readthedocs.io/>`__
- Source builds: `Cython <https://cython.org/>`__ and a compiler.

Getting started
---------------

.. code-block:: python

   >>> from numpythia import Pythia, hepmc_write, hepmc_read
   >>> from numpythia import STATUS, HAS_END_VERTEX, ABS_PDG_ID
   >>> from numpythia.testcmnd import get_cmnd
   >>> from numpy.testing import assert_array_equal

   >>> pythia = Pythia(get_cmnd('w'), random_state=1)

   >>> selection = ((STATUS == 1) & ~HAS_END_VERTEX &
               (ABS_PDG_ID != 12) & (ABS_PDG_ID != 14) & (ABS_PDG_ID != 16))

   >>> # generate events while writing to ascii hepmc
   >>> for event in hepmc_write('events.hepmc', pythia(events=1)):
   >>>    array1 = event.all(selection)

   >>> # read the same event back from ascii hepmc
   >>> for event in hepmc_read('events.hepmc'):
   >>>    array2 = event.all(selection)

   >>> assert_array_equal(array1, array2)
   True

The dtype of any array of particle information is:

.. code-block:: python

   np.dtype([('E', 'f8'), ('px', 'f8'), ('py', 'f8'), ('pz', 'f8'), ('pt', 'f8'),
             ('mass', 'f8'), ('rap', 'f8'), ('eta', 'f8'), ('theta', 'f8'),
             ('phi', 'f8'), ('prodx', 'f8'), ('prody', 'f8'), ('prodz', 'f8'),
             ('prodt', 'f8'), ('pdgid', 'i4'), ('status', 'i4')])

Also see `pyjet <https://github.com/scikit-hep/pyjet>`_ for jet clustering.

Tutorial
--------

Setting PYTHIA
~~~~~~~~~~~~~~

PYTHIA settings can be passed in one of three ways: through the `**kwargs` arguments of the constructor `Pythia(..., **kwargs)`:

.. code-block:: python

   >>> pythia = Pythia(..., Beams_eCM=13000.)

Or as a dictionary:

.. code-block:: python

   >>> pythia = Pythia(..., params={'Beams:eCM':  13000.})

Or via a Python command file:

.. code-block:: python

   >>> pythia = Pythia(config='path/to/config.cmd')

The full list of settings can be found on the  `PYTHIA homepage <http://home.thep.lu.se/Pythia/>`_.

Note that the ":" in settings names is replaced by a "_"  if using `kwargs`.
`kwargs` take precedence over `params` and they both take precedence over `config`.
Example config files can be found under the `numpythia.testcmnd` directory.

Generate events
~~~~~~~~~~~~~~~

To generate events do

.. code-block:: python

   >>> events = pythia(events=100)
   >>> events
   <generator at 0x10cf06f78>

where **events** is a generator of ``GenEvent`` containing all the generated particles.

Generated particles can be accessed through the ``all``, ``first`` and ``last``
methods which have two optional arguments ``selection`` and ``return_hepmc``.
Selection is a filter or a combination of filters with bitwise operations (as
shown in the *getting started* example) applied on the particles in the event.
The available filters are

.. code-block:: python

    STATUS, PDG_ID, ABS_PDG_ID, HAS_END_VERTEX, HAS_PRODUCTION_VERTEX,
    HAS_SAME_PDG_ID_DAUGHTER, IS_STABLE, IS_BEAM

``return_hepmc`` is by default set to ``False`` when using ``all``:

.. code-block:: python

   >>> for e in events:
   >>>     array = e.all(selection)

returns an array of particles, with the dtype described above. ``return_hepmc`` is
by default set to ``True`` for ``first`` and ``last``:

.. code-block:: python

    >>> for e in events:
    >>>     gen_part_f = e.first(selection)
    >>>     gen_part_l = e.last(selection)

returns a ``GenParticle``.

Generated particle
~~~~~~~~~~~~~~~~~~

``GenParticle`` is the numpythia interface of
`HepMC::GenParticle <http://lcgapp.cern.ch/project/simu/HepMC/205/html/classHepMC_1_1GenParticle.html>`_,
and has the following attributes

.. code-block:: python

    pid, status, e, px, py, pz, pt, eta, phi, mass, theta, rap

``GenParticle`` also has the following methods ``parents``, ``children``, ``ancestors``,
``descendants`` and ``siblings`` both with the two optional arguments ``selection``
and ``return_hepmc`` described before. For instance:

.. code-block:: python

    >>> for e in events:
    >>>     w = e.last((ABS_PDG_ID == 24) & HAS_END_VERTEX))
    >>>     w.children()
    array([(240.60708981, 115.76101664, 126.16766767, -169.03439984, 171.22760682, 0.5, -0.87228439, -0.87228739, 2.34974894, 0.82838703, 0., 0., 0., 0.,  3, 23),
       ( 52.59241372,   9.21296404,  50.77873929,  -10.01763001,  51.60774235, 1.5, -0.19283178, -0.19291222, 1.76252302, 1.39131523, 0., 0., 0., 0., -4, 23)],
      dtype=[('E', '<f8'), ('px', '<f8'), ('py', '<f8'), ('pz', '<f8'), ('pT', '<f8'), ('mass', '<f8'), ('rap', '<f8'), ('eta', '<f8'), ('theta', '<f8'), ('phi', '<f8'), ('prodx', '<f8'), ('prody', '<f8'), ('prodz', '<f8'), ('prodt', '<f8'), ('pdgid', '<i4'), ('status', '<i4')])
