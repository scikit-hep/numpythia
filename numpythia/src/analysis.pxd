from libcpp cimport bool

cimport pythia as Pythia

cdef extern from "Pythia8/Analysis.h" namespace "Pythia8":
    cdef cppclass Sphericity:
        Sphericity() 
        Sphericity(double powerIn, int selectIn)
        bool analyze(const Pythia.Event& event) 
        double sphericity()