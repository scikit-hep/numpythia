# cython: experimental_cpp_class_def=True, c_string_type=str, c_string_encoding=ascii

import numpy as np
cimport numpy as np
np.import_array()

cimport cython
from cython.operator cimport dereference as deref

from libc.stdlib cimport malloc, free

from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.string cimport string, const_char
from libcpp.cast cimport static_cast

from cpython cimport PyObject
from cpython.cobject cimport (PyCObject_AsVoidPtr,
                              PyCObject_Check,
                              PyCObject_FromVoidPtr)

from libcpp.memory cimport shared_ptr
import os

cimport pythia as Pythia
cimport hepmc as HepMC
cimport numpythia

cdef extern from "2to3.h":
    pass

DTYPE = np.float64
ctypedef np.float64_t DTYPE_t

DTYPE_EP = np.dtype([('E', DTYPE), ('px', DTYPE), ('py', DTYPE), ('pz', DTYPE)])
DTYPE_PTEPM = np.dtype([('pT', DTYPE), ('eta', DTYPE), ('phi', DTYPE), ('mass', DTYPE)])
DTYPE_PARTICLE = np.dtype([('E', DTYPE), ('px', DTYPE), ('py', DTYPE), ('pz', DTYPE), ('mass', DTYPE),
                           ('prodx', DTYPE), ('prody', DTYPE), ('prodz', DTYPE), ('prodt', DTYPE),
                           ('pdgid', np.int32), ('status', np.int32)])

ALL = HepMC.FIND_ALL
FIRST = HepMC.FIND_FIRST
LAST = HepMC.FIND_LAST


cdef class FilterList:
    cdef HepMC.FilterList _filterlist

    def __and__(FilterList self, other):
        filterlist = FilterList()
        filterlist._filterlist.extend(self._filterlist)
        if isinstance(other, BooleanFilter):
            filterlist._filterlist.append(deref(static_cast["const HepMC.Filter*"]((<BooleanFilter> other)._filter)))
        elif isinstance(other, FilterList):
            filterlist._filterlist.extend((<FilterList> other)._filterlist)
        else:
            raise TypeError("can only combine boolean filters")
        return filterlist

cdef class Filter:
    cdef bool own
    cdef const HepMC.FilterBase* _filter

    cdef init(self, const HepMC.FilterBase& _filter, bool own):
        self._filter = &_filter
        self.own = own

    def __dealloc__(self):
        if self.own:
            del self._filter

    def __and__(Filter self, other):
        filterlist = FilterList()
        if isinstance(other, BooleanFilter):
            filterlist._filterlist = HepMC.FilterList(
                deref(static_cast["const HepMC.Filter*"](self._filter)),
                deref(static_cast["const HepMC.Filter*"]((<Filter> other)._filter)))
        elif isinstance(other, FilterList):
            filterlist._filterlist = HepMC.FilterList(deref(static_cast["const HepMC.Filter*"](self._filter)))
            filterlist._filterlist.extend((<FilterList> other)._filterlist)
        else:
            raise TypeError("can only combine boolean filters")
        return filterlist

cdef class IntegerFilter(Filter):
    def __richcmp__(IntegerFilter self, int value, int op):
        result = BooleanFilter()
        if op == 0:
            result._filter = new HepMC.Filter(deref(self._filter) < value)
        elif op == 2:
            result._filter = new HepMC.Filter(deref(self._filter) == value)
        elif op == 4:
            result._filter = new HepMC.Filter(deref(self._filter) > value)
        elif op == 1:
            result._filter = new HepMC.Filter(deref(self._filter) <= value)
        elif op == 3:
            result._filter = new HepMC.Filter(deref(self._filter) != value)
        elif op == 5:
            result._filter = new HepMC.Filter(deref(self._filter) >= value)
        result.own = True
        return result

cdef class BooleanFilter(Filter):
    def __invert__(self):
        result = BooleanFilter()
        # Ouch
        result._filter = new HepMC.Filter(not deref(static_cast["const HepMC.Filter*"](self._filter)))
        result.own = True
        return result

# filters with integer parameter
cpdef IntegerFilter STATUS = IntegerFilter()
STATUS.init(HepMC.STATUS, False)
cpdef IntegerFilter PDG_ID = IntegerFilter()
PDG_ID.init(HepMC.PDG_ID, False)
cpdef IntegerFilter ABS_PDG_ID = IntegerFilter()
ABS_PDG_ID.init(HepMC.ABS_PDG_ID, False)

# filters with boolean parameter
cpdef BooleanFilter HAS_END_VERTEX = BooleanFilter()
HAS_END_VERTEX.init(HepMC.HAS_END_VERTEX, False)
cpdef BooleanFilter HAS_PRODUCTION_VERTEX = BooleanFilter()
HAS_PRODUCTION_VERTEX.init(HepMC.HAS_PRODUCTION_VERTEX, False)
cpdef BooleanFilter HAS_SAME_PDG_ID_DAUGHTER = BooleanFilter()
HAS_SAME_PDG_ID_DAUGHTER.init(HepMC.HAS_SAME_PDG_ID_DAUGHTER, False)
cpdef BooleanFilter IS_STABLE = BooleanFilter()
IS_STABLE.init(HepMC.IS_STABLE, False)
cpdef BooleanFilter IS_BEAM = BooleanFilter()
IS_BEAM.init(HepMC.IS_BEAM, False)

FILTERS = {
    'STATUS': STATUS,
    'PDG_ID': PDG_ID,
    'ABS_PDG_ID': ABS_PDG_ID,
    'HAS_END_VERTEX': HAS_END_VERTEX,
    'HAS_PRODUCTION_VERTEX': HAS_PRODUCTION_VERTEX,
    'HAS_SAME_PDG_ID_DAUGHTER': HAS_SAME_PDG_ID_DAUGHTER,
    'IS_STABLE': IS_STABLE,
    'IS_BEAM': IS_BEAM,
}


cdef class MCInput:
    cdef np.ndarray weights

    cdef int get_num_weights(self):
        return 0

    property weighted:
        def __get__(self):
            return self.get_num_weights() > 0

    property num_weights:
        def __get__(self):
            return self.get_num_weights()

    cdef bool get_next_event(self) except *:
        return False

    cdef HepMC.GenEvent* get_hepmc(self):
        pass

    """
    cdef void to_pseudojet(self, vector[PseudoJet]& particles, float eta_max):
        pass

    cdef void to_delphes(self, Delphes* modular_delphes,
                         TObjArray* delphes_all_particles,
                         TObjArray* delphes_stable_particles,
                         TObjArray* delphes_partons):
        pass
    """

    cdef void finish(self):
        pass


cdef class PythiaInput(MCInput):

    cdef Pythia.Pythia* pythia
    #cdef Pythia.VinciaPlugin* vincia_plugin
    cdef Pythia.UserHooks* userhooks
    cdef HepMC.GenEvent* hepmc_event
    cdef int verbosity
    cdef int cut_on_pdgid
    cdef float pdgid_pt_min
    cdef float pdgid_pt_max
    cdef string shower

    def __cinit__(self, string config, string xmldoc,
                  int random_state=0, float beam_ecm=13000.,
                  int cut_on_pdgid=0,
                  float pdgid_pt_min=-1, float pdgid_pt_max=-1,
                  object params_dict=None, int verbosity=1,
                  string shower='',
                  **kwargs):

        cdef int i
        cdef double mPDF

        self.pythia = new Pythia.Pythia(xmldoc, False)

        # Initialize pointers to NULL
        #self.vincia_plugin = NULL
        self.userhooks = NULL
        self.hepmc_event = NULL

        if verbosity > 0:
            self.pythia.readString("Init:showProcesses = on")
            self.pythia.readString("Init:showChangedSettings = on")
        else:
            self.pythia.readString("Init:showProcesses = off")
            self.pythia.readString("Init:showChangedSettings = off")

        if verbosity > 1:
            self.pythia.readString("Init:showMultipartonInteractions = on")
            self.pythia.readString("Init:showChangedParticleData = on")
            self.pythia.readString("Next:numberShowInfo = 1")
            self.pythia.readString("Next:numberShowProcess = 1")
            self.pythia.readString("Next:numberShowEvent = 1")
        else:
            self.pythia.readString("Init:showMultipartonInteractions = off")
            self.pythia.readString("Init:showChangedParticleData = off")
            self.pythia.readString("Next:numberShowInfo = 0")
            self.pythia.readString("Next:numberShowProcess = 0")
            self.pythia.readString("Next:numberShowEvent = 0")

        # next read user config that may override options above
        #if shower == 'vincia':
            #self.vincia_plugin = new VinciaPlugin(self.pythia, config)

        #else:  # default Pythia shower
        # Read config
        self.pythia.readFile(config)

        # __init__ arguments will always override the config
        self.pythia.readString('Beams:eCM = {0}'.format(beam_ecm))
        self.pythia.readString('Random:setSeed = on')
        self.pythia.readString('Random:seed = {0}'.format(random_state))

        if params_dict is not None:
            for param, value in params_dict.items():
                self.pythia.readString('{0} = {1}'.format(param, value))
        for param, value in kwargs.items():
            self.pythia.readString('{0} = {1}'.format(param.replace('_', ':'), value))

        #if shower == 'vincia':
            ## vincia calls pythia's init
            ## but we lose the bool return value of Pythia's init
            ## if init fails there, pythia.next() will anyway abort
            ## TODO: follow this up with Skands et al
            #success = self.vincia_plugin.init()
        #else:
        if not self.pythia.init():
            raise RuntimeError("PYTHIA did not successfully initialize")

        self.cut_on_pdgid = cut_on_pdgid
        self.pdgid_pt_min = pdgid_pt_min
        self.pdgid_pt_max = pdgid_pt_max
        self.verbosity = verbosity
        self.shower = shower

    def __dealloc__(self):
        del self.hepmc_event
        del self.pythia
        #del self.vincia_plugin
        del self.userhooks

    cdef int get_num_weights(self):
        """
        Accessing the nominal and uncertainty event weights

        http://home.thep.lu.se/Pythia/pythia82html/EventInformation.html

        The weight assigned to the current event. Is normally 1 and thus
        uninteresting. However, there are several cases where one may have
        nontrivial event weights.

        http://home.thep.lu.se/Pythia/pythia82html/Variations.html

        During the event generation, uncertainty weights will be calculated for
        each variation. The resulting alternative weights for the event are
        accessible through the Pythia::info.weight(int iWeight=0) method. The
        baseline weight for each event (normally unity for an ordinary
        unweighted event sample) is not modified and corresponds to iWeight =
        0. The uncertainty-variation weights are thus enumerated starting from
        iWeight = 1 for the first variation up to N for the last variation, in
        the order they were specified in UncertaintyBands:List.

        The total number of variations that have been defined, N, can be
        queried using Pythia::info.nWeights().
        """
        return self.pythia.info.nWeights()

    cdef bool get_next_event(self) except *:
        # generate event and quit if failure
        if not self.pythia.next():
            raise RuntimeError("PYTHIA event generation aborted prematurely")
        if self.num_weights > 0:
            self.weights = np.empty(self.num_weights, dtype=DTYPE)
            for iweight in range(self.num_weights):
                self.weights[iweight] = self.pythia.info.weight(iweight)
        return True

    cdef HepMC.GenEvent* get_hepmc(self):
        del self.hepmc_event
        self.hepmc_event = numpythia.pythia_to_hepmc(self.pythia)
        return self.hepmc_event

    """
    cdef void to_pseudojet(self, vector[PseudoJet]& particles, float eta_max):
        pythia_to_pseudojet(self.pythia.event, particles, eta_max)

    cdef void to_delphes(self, Delphes* delphes,
                         TObjArray* all_particles,
                         TObjArray* stable_particles,
                         TObjArray* partons):
        # convert Pythia particles into Delphes candidates
        pythia_to_delphes(self.pythia.event, delphes,
                          all_particles,
                          stable_particles,
                          partons)
    """

    cdef void finish(self):
        if self.verbosity > 0:
            self.pythia.stat()


cdef class HepMCInput(MCInput):

    cdef string filename
    cdef HepMC.ReaderAscii* hepmc_reader
    cdef HepMC.GenEvent* event
    #cdef TDatabasePDG *pdg

    def __cinit__(self, string filename):
        self.filename = filename
        self.hepmc_reader = new HepMC.ReaderAscii(filename)
        self.event = NULL
        #self.pdg = TDatabasePDG_Instance()

    def __dealloc__(self):
        del self.event
        del self.hepmc_reader

    cdef bool get_next_event(self) except *:
        del self.event
        self.event = new HepMC.GenEvent()
        return self.hepmc_reader.read_event(deref(self.event))

    cdef HepMC.GenEvent* get_hepmc(self):
        return self.event

    """
    cdef void to_pseudojet(self, vector[PseudoJet]& particles, float eta_max):
        hepmc_to_pseudojet(self.event[0], particles, eta_max)

    cdef void to_delphes(self, Delphes* delphes,
                         TObjArray* all_particles,
                         TObjArray* stable_particles,
                         TObjArray* partons):
        # convert Pythia particles into Delphes candidates
        hepmc_to_delphes(self.event, self.pdg,
                         delphes,
                         all_particles,
                         stable_particles,
                         partons)
    """

    def estimate_num_events(self, int sample_size=1000):
        """
        Getting the exact number of events in a HepMC file is too expensive
        since this involves counting total number of lines beginning with "E"
        and the file can be GB in size. Even "wc -l file.hepmc" takes forever.
        Instead we can estimate the number of events by averaging the event
        sizes for the first N events and then dividing the total file size by
        that. This estimate may be used to report a progress bar as the reader
        loops over events.
        """
        cdef np.ndarray sizes = np.empty(sample_size, dtype=np.int32)
        cdef int num_found = 0
        cdef long long prev_location = 0
        filesize = os.path.getsize(self.filename)
        with open(self.filename, 'r') as infile:
            for line in infile:
                if line[0] == 'E':
                    if num_found > 0:
                        sizes[num_found - 1] = infile.tell() - prev_location
                    if num_found == sample_size:
                        break
                    num_found += 1
                    prev_location = infile.tell()
            else:
                return num_found
        return long(filesize / np.average(sizes))


@cython.boundscheck(False)
@cython.wraparound(False)
def generate(MCInput gen_input, int n_events, object find, HepMC.FilterType select, string write_to, bool weighted=False):
    """
    Generate events (or read HepMC) and yield numpy arrays of particles
    If weights are enabled, this function will yield the particles and weights array
    """
    cdef np.ndarray particle_array
    cdef HepMC.GenEvent* event
    cdef HepMC.WriterAscii* hepmc_writer = NULL
    cdef vector[HepMC.SmartPointer[HepMC.GenParticle]] particles
    cdef HepMC.FindParticles* search = NULL
    cdef int ievent = 0;
    if n_events < 0:
        ievent = n_events - 1
    if not write_to.empty():
        hepmc_writer = new HepMC.WriterAscii(write_to)
    if isinstance(find, BooleanFilter):
        find = FilterList(find)
    if find is not None and not isinstance(find, FilterList):
        raise TypeError("find must be a boolean expression of Filters")
    try:
        while ievent < n_events:
            if not gen_input.get_next_event():
                continue
            # We don't own event here. MCInput will delete it.
            event = gen_input.get_hepmc()
            if hepmc_writer != NULL:
                hepmc_writer.write_event(deref(event))

            if find is not None:
                search = new HepMC.FindParticles(deref(event), select, (<FilterList>find)._filterlist)
                particles = search.results()
                del search
            else:
                particles = event.particles()

            particle_array = np.empty((particles.size(),), dtype=DTYPE_PARTICLE)
            numpythia.hepmc_to_array(particles, <char*> particle_array.data, <unsigned int> particle_array.itemsize)
            if weighted:
                yield particle_array, gen_input.weights
            else:
                yield particle_array
            if n_events > 0:
                ievent += 1
    except:
        raise
    finally:
        del hepmc_writer
    gen_input.finish()
