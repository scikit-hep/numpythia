.. -*- mode: rst -*-

numpythia: The interface between PYTHIA and NumPy
=================================================

numpythia provides an interface between `PYTHIA
<http://home.thep.lu.se/~torbjorn/Pythia.html>`_ and `NumPy
<http://www.numpy.org/>`_ allowing you to generate events as NumPy arrays of
particle four-momenta. The interface code is written in Cython that then
becomes compiled C++, so it's fast.
