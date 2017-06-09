from libcpp cimport bool
from libcpp.string cimport string

cdef extern from "Pythia8/Pythia.h" namespace "Pythia8":
    cdef cppclass Event:
        pass

    cdef cppclass Info:
        int nWeights()
        double weight(int)

    cdef cppclass Settings:
        void addFlag(string, bool) 
        void addMode(string, int, bool, bool, int, int)
        bool flag(string)
        int mode(string)
        double parm(string)

    cdef cppclass TimeShower:
        pass

    cdef cppclass SpaceShower:
        pass

    cdef cppclass UserHooks:
        pass

    cdef cppclass MergingHooks:
        pass

    cdef cppclass ParticleData:
        pass

    cdef cppclass Rndm:
        pass

    cdef cppclass PartonSystems:
        pass

    cdef cppclass BeamParticle:
        int id()
        double mQuarkPDF(int)

    cdef cppclass Pythia:
        Event event
        Info info
        Settings settings
        ParticleData particleData
        Rndm rndm
        PartonSystems partonSystems
        Pythia(string, bool)
        bool readString(string)
        bool readFile(string)
        bool init()
        bool next()
        void stat()
        bool setShowerPtr(TimeShower*, TimeShower*, SpaceShower*)
        bool setUserHooksPtr(UserHooks*)

#cdef extern from "Vincia/Vincia.h" namespace "Vincia":
    #cdef cppclass VinciaPlugin:
        #VinciaPlugin(Pythia*, string)
        #void init()
