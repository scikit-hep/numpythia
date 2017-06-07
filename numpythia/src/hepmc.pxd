
cdef extern from "HepMC/GenEvent.h" namespace "HepMC":
    cdef cppclass GenEvent:
        pass

cdef extern from "HepMC/SimpleVector.h" namespace "HepMC":
    cdef cppclass ThreeVector:
        double x()
        double y()
        double z()
        double phi()
        double theta()

    cdef cppclass FourVector:
        double px()
        double py()
        double pz()
        double e()
        double m()
        double perp()
        double phi()
        double eta()
        double theta()

cdef extern from "HepMC/GenVertex.h" namespace "HepMC":
    cdef cppclass GenVertex:
        ThreeVector point3d()

cdef extern from "HepMC/GenParticle.h" namespace "HepMC":
    cdef cppclass GenParticle:
        int pdg_id()
        int status()
        FourVector momentum()
        GenVertex* end_vertex()
        GenVertex* production_vertex()

cdef extern from "HepMC/IO_GenEvent.h" namespace "HepMC":
    cdef cppclass IO_GenEvent:
        GenEvent* read_next_event()
        void write_event(GenEvent*)
