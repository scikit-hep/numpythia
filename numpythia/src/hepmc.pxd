from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "HepMC/Data/SmartPointer.h" namespace "HepMC":
    cdef cppclass SmartPointer[T]:
        pass

cdef extern from "HepMC/Units.h" namespace "HepMC::Units":
    cdef enum MomentumUnit "HepMC::Units::MomentumUnit":
        MEV,
        GEV

    cdef enum LengthUnit "HepMC::Units::LengthUnit":
        MM,
        CM

cdef extern from "HepMC/GenEvent.h" namespace "HepMC":
    cdef cppclass GenEvent:
        vector[SmartPointer[GenParticle]]& particles()

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

cdef extern from "HepMC/Search/FilterBase.h" namespace "HepMC":
    cdef cppclass FilterBase:
        @staticmethod
        FilterBase init_status()
        @staticmethod
        FilterBase init_pdg_id()
        @staticmethod
        FilterBase init_abs_pdg_id()
        @staticmethod
        Filter init_has_end_vertex()
        @staticmethod
        Filter init_has_production_vertex()
        @staticmethod
        Filter init_has_same_pdg_id_daughter()
        @staticmethod
        Filter init_is_stable()
        @staticmethod
        Filter init_is_beam()
        Filter operator==(int)
        Filter operator!=(int)
        Filter operator> (int)
        Filter operator>=(int)
        Filter operator< (int)
        Filter operator<=(int)

    cdef const FilterBase STATUS
    cdef const FilterBase PDG_ID
    cdef const FilterBase ABS_PDG_ID

cdef extern from "HepMC/Search/Filter.h" namespace "HepMC":
    cdef cppclass Filter(FilterBase):
        Filter (const Filter&)
        Filter operator!()
    
    cdef const Filter HAS_END_VERTEX
    cdef const Filter HAS_PRODUCTION_VERTEX
    cdef const Filter HAS_SAME_PDG_ID_DAUGHTER
    cdef const Filter IS_STABLE
    cdef const Filter IS_BEAM

cdef extern from "HepMC/Search/FilterList.h" namespace "HepMC":
    cdef cppclass FilterList:
        FilterList()
        FilterList(const Filter&)
        FilterList(const Filter&, const Filter&)
        const vector[Filter]& filters()
        FilterList& extend(FilterList&)
        FilterList& append(Filter&)

cdef extern from "HepMC/Search/FindParticles.h" namespace "HepMC":
    cdef enum FilterType "HepMC::FilterType":
        FIND_ALL,
        FIND_FIRST,
        FIND_LAST

    cdef cppclass FindParticles:
        FindParticles(GenEvent&, FilterType)
        FindParticles(GenEvent&, FilterType, FilterList)
        vector[SmartPointer[GenParticle]] results()
