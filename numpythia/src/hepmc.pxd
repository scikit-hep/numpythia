from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "HepMC/Units.h" namespace "HepMC::Units":
    cdef enum MomentumUnit "HepMC::Units::MomentumUnit":
        MEV,
        GEV

    cdef enum LengthUnit "HepMC::Units::LengthUnit":
        MM,
        CM

cdef extern from "HepMC/GenEvent.h" namespace "HepMC":
    cdef cppclass GenEvent:
        pass

cdef extern from "HepMC/FourVector.h" namespace "HepMC":
    cdef cppclass FourVector:
        double x()
        double y()
        double z()
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
        FourVector position()

cdef extern from "HepMC/GenParticle.h" namespace "HepMC":
    cdef cppclass GenParticle:
        int pdg_id()
        int status()
        FourVector momentum()
        GenVertex* end_vertex()
        GenVertex* production_vertex()

cdef extern from "HepMC/ReaderAscii.h" namespace "HepMC":
    cdef cppclass ReaderAscii:
        ReaderAscii(string filename)
        bool read_event(GenEvent&)

cdef extern from "HepMC/WriterAscii.h" namespace "HepMC":
    cdef cppclass WriterAscii:
        WriterAscii(string filename)
        void write_event(GenEvent&)
