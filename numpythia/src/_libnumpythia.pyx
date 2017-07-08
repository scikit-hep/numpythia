# cython: experimental_cpp_class_def=True, c_string_type=str, c_string_encoding=ascii

# TODO:
# Wrap HepMC Writer and remove write_to
# Wrap GenEvent and yield that

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

from .extern.six import string_types
from pkg_resources import resource_filename
from fnmatch import fnmatch

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

ANCESTORS = HepMC.ANCESTORS
DESCENDANTS = HepMC.DESCENDANTS
PARENTS = HepMC.PARENTS
CHILDREN = HepMC.CHILDREN
SIBLINGS = HepMC.PRODUCTION_SIBLINGS


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


cdef inline np.ndarray particles_to_array(vector[HepMC.SmartPointer[HepMC.GenParticle]] particles):
    cdef np.ndarray particle_array = np.empty((particles.size(),), dtype=DTYPE_PARTICLE)
    numpythia.hepmc_to_array(particles, <char*> particle_array.data, <unsigned int> particle_array.itemsize)
    return particle_array


cdef inline object particle_find(HepMC.SmartPointer[HepMC.GenParticle]& particle, object selection,
                                 HepMC.Relationship mode, bool return_hepmc):
    cdef HepMC.FindParticles* search
    if selection is None:
        search = new HepMC.FindParticles(particle, mode)
    else:
        if isinstance(selection, BooleanFilter):
            selection = FilterList(selection)
        elif not isinstance(selection, FilterList):
            raise TypeError("find must be a boolean expression of Filters")
        search = new HepMC.FindParticles(particle, mode, (<FilterList> selection)._filterlist)
    cdef vector[HepMC.SmartPointer[HepMC.GenParticle]] particles = search.results()
    del search
    if return_hepmc:
        return vector_to_list(particles)
    return particles_to_array(particles)


cdef inline object event_find(shared_ptr[HepMC.GenEvent]& event, object selection,
                              HepMC.FilterType mode, bool return_hepmc):
    cdef list py_particles
    if selection is None:
        if return_hepmc:
            py_particles = vector_to_list(deref(event).particles())
            if mode == FIRST or mode == LAST:
                return py_particles[0] if py_particles else None
            return py_particles
        return particles_to_array(deref(event).particles())
    if isinstance(selection, BooleanFilter):
        selection = FilterList(selection)
    elif not isinstance(selection, FilterList):
        raise TypeError("find must be a boolean expression of Filters")
    cdef HepMC.FindParticles* search = new HepMC.FindParticles(deref(event), mode, (<FilterList> selection)._filterlist)
    cdef vector[HepMC.SmartPointer[HepMC.GenParticle]] particles = search.results()
    del search
    if return_hepmc:
        py_particles = vector_to_list(particles)
        if mode == FIRST or mode == LAST:
            return py_particles[0] if py_particles else None
        return py_particles
    return particles_to_array(particles)


cdef class GenParticle:
    cdef HepMC.SmartPointer[HepMC.GenParticle] particle

    @staticmethod
    cdef inline GenParticle wrap(HepMC.SmartPointer[HepMC.GenParticle]& particle):
        cdef GenParticle wrapped_particle = GenParticle()
        wrapped_particle.particle = particle
        return wrapped_particle

    def parents(self, object selection=None, bool return_hepmc=False):
        return particle_find(self.particle, selection, mode=PARENTS, return_hepmc=return_hepmc)

    def children(self, object selection=None, bool return_hepmc=False):
        return particle_find(self.particle, selection, mode=CHILDREN, return_hepmc=return_hepmc)

    def ancestors(self, object selection=None, bool return_hepmc=False):
        return particle_find(self.particle, selection, mode=ANCESTORS, return_hepmc=return_hepmc)

    def descendants(self, object selection=None, bool return_hepmc=False):
        return particle_find(self.particle, selection, mode=DESCENDANTS, return_hepmc=return_hepmc)

    def siblings(self, object selection=None, bool return_hepmc=False):
        return particle_find(self.particle, selection, mode=SIBLINGS, return_hepmc=return_hepmc)

    @property
    def pid(self):
        return deref(self.particle).pid()

    @property
    def status(self):
        return deref(self.particle).status()

    @property
    def e(self):
        return deref(self.particle).momentum().e()

    @property
    def px(self):
        return deref(self.particle).momentum().px()

    @property
    def py(self):
        return deref(self.particle).momentum().py()

    @property
    def pz(self):
        return deref(self.particle).momentum().pz()

    @property
    def pt(self):
        return deref(self.particle).momentum().pt()

    @property
    def eta(self):
        return deref(self.particle).momentum().eta()

    @property
    def phi(self):
        return deref(self.particle).momentum().phi()

    @property
    def mass(self):
        return deref(self.particle).momentum().m()

    @property
    def theta(self):
        return deref(self.particle).momentum().theta()

    @property
    def rap(self):
        return deref(self.particle).momentum().rap()

    def __repr__(self):
        return "{0}(e={1:.3f}, px={2:.3f}, py={3:.3f}, pz={4:.3f}, mass={5:.3f}, pid={6:d}, status={7:d})".format(
            self.__class__.__name__, self.e, self.px, self.py, self.pz, self.mass, self.pid, self.status)


cdef inline list vector_to_list(vector[HepMC.SmartPointer[HepMC.GenParticle]]& particles):
    py_particles = []
    for particle in particles:
        py_particles.append(GenParticle.wrap(particle))
    return py_particles


cdef class GenEvent:
    cdef shared_ptr[HepMC.GenEvent] event
    cdef public np.ndarray weights

    @staticmethod
    cdef inline GenEvent wrap(shared_ptr[HepMC.GenEvent]& event):
        cdef GenEvent wrapped_event = GenEvent()
        cdef vector[double] weights = deref(event).weights()
        cdef np.ndarray weights_array = np.empty(weights.size(), dtype=np.float64)
        for iweight in range(weights.size()):
            weights_array[iweight] = weights[iweight]
        wrapped_event.event = event
        wrapped_event.weights = weights_array
        return wrapped_event

    @staticmethod
    cdef inline GenEvent wrap_pythia(Pythia.Pythia& pythia):
        cdef HepMC.Pythia8ToHepMC3 py2hepmc
        # Suppress warnings
        py2hepmc.set_print_inconsistency(False)
        cdef shared_ptr[HepMC.GenEvent] event = shared_ptr[HepMC.GenEvent](new HepMC.GenEvent(HepMC.GEV, HepMC.MM))
        if not py2hepmc.fill_next_event(pythia, &deref(event)):
            raise RuntimeError("unable to convert PYTHIA event to HepMC")
        return GenEvent.wrap(event)

    def all(self, object selection=None, bool return_hepmc=False):
        return event_find(self.event, selection, ALL, return_hepmc)

    def first(self, object selection=None, bool return_hepmc=True):
        return event_find(self.event, selection, FIRST, return_hepmc)

    def last(self, object selection=None, bool return_hepmc=True):
        return event_find(self.event, selection, LAST, return_hepmc)


cdef class _Pythia:
    cdef Pythia.Pythia* pythia
    #cdef Pythia.VinciaPlugin* vincia_plugin
    cdef Pythia.UserHooks* userhooks
    cdef int verbosity
    cdef string shower

    def __cinit__(self, string config,
                  int random_state=0,
                  object params_dict=None,
                  int verbosity=1,
                  string shower='',
                  **kwargs):

        cdef int i
        cdef double mPDF

        xmldoc = resource_filename('numpythia', 'src/extern/pythia8226/share')
        self.pythia = new Pythia.Pythia(xmldoc, False)

        # Initialize pointers to NULL
        #self.vincia_plugin = NULL
        self.userhooks = NULL

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

        self.verbosity = verbosity
        self.shower = shower

    def __dealloc__(self):
        del self.pythia
        #del self.vincia_plugin
        del self.userhooks

    @property
    def nweights(self):
        return self.pythia.info.nWeights()

    @property
    def weight_labels(self):
        cdef list labels = []
        cdef int iweight
        for iweight in range(self.pythia.info.nWeights()):
            labels.append(self.pythia.info.weightLabel(iweight))
        return labels

    cdef bool get_next_event(self) except *:
        # generate event and quit if failure
        if not self.pythia.next():
            raise RuntimeError("PYTHIA event generation aborted prematurely")
        return True

    cdef GenEvent get_hepmc(self):
        return GenEvent.wrap_pythia(deref(self.pythia))

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

    def __iter__(self):
        for event in self():
            yield event

    def __call__(self, int events=-1):
        cdef int ievent = 0;
        if events < 0:
            ievent = events - 1
        while ievent < events:
            if not self.get_next_event():
                continue
            yield self.get_hepmc()
            if events > 0:
                ievent += 1
        if self.verbosity > 0:
            self.pythia.stat()


cdef class WriterAscii:
    cdef HepMC.WriterAscii* hepmc_writer

    def __cinit__(self, string filename):
        self.hepmc_writer = new HepMC.WriterAscii(filename)

    def __dealloc__(self):
        self.hepmc_writer.close()
        del self.hepmc_writer

    def write(self, GenEvent event):
        self.hepmc_writer.write_event(deref(event.event))


cdef class ReaderAscii:
    cdef string filename
    cdef HepMC.ReaderAscii* hepmc_reader
    cdef shared_ptr[HepMC.GenEvent] event

    def __cinit__(self, string filename):
        self.filename = filename
        self.hepmc_reader = new HepMC.ReaderAscii(filename)

    def __dealloc__(self):
        self.hepmc_reader.close()
        del self.hepmc_reader

    def __iter__(self):
        while not self.hepmc_reader.failed():
            self.event.reset(new HepMC.GenEvent())
            self.hepmc_reader.read_event(deref(self.event))
            if self.hepmc_reader.failed():
                break
            yield GenEvent.wrap(self.event)

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
