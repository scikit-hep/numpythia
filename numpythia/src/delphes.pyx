import tempfile


cdef class DelphesWrapper:
    cdef ExRootConfReader* config_reader
    cdef Delphes* delphes
    cdef TObjArray* all_particles
    cdef TObjArray* stable_particles
    cdef TObjArray* partons
    cdef TObjArray* input_array
    cdef TDatabasePDG *pdg

    def __cinit__(self, string config, int random_state, string objects):
        self.pdg = TDatabasePDG_Instance()
        self.config_reader = new ExRootConfReader()
        self.config_reader.ReadFile(config.c_str())
        # Set Delphes' random seed. Only possible through a config file...
        with tempfile.NamedTemporaryFile() as tmp:
            tmp.write("set RandomSeed {0:d}\n".format(random_state))
            tmp.flush()
            # Give the reader a copy of the string since it has been seen to
            # clobber the string (adding null bytes)
            self.config_reader.ReadFile('%s' % tmp.name)
        self.delphes = new Delphes("Delphes")
        self.delphes.SetConfReader(self.config_reader)
        self.all_particles = self.delphes.ExportArray("allParticles")
        self.stable_particles = self.delphes.ExportArray("stableParticles")
        self.partons = self.delphes.ExportArray("partons")
        self.delphes.InitTask()
        self.input_array = self.delphes.ImportArray(objects.c_str())

    def __dealloc__(self):
        del self.delphes
        del self.config_reader

    cdef np.ndarray reconstruct_mc(self, MCInput gen_input):
        cdef np.ndarray candidates
        self.delphes.Clear()
        # convert generator particles into Delphes candidates
        gen_input.to_delphes(self.delphes,
                             self.all_particles,
                             self.stable_particles,
                             self.partons)
        # run Delphes reconstruction
        self.delphes.ProcessTask()
        candidates = np.empty((self.input_array.GetEntries(),), dtype=dtype_fourvect)
        delphes_to_array(self.input_array, <DTYPE_t*> candidates.data)
        return candidates

    cdef np.ndarray reconstruct_array(self, np.ndarray particles):
        cdef np.ndarray candidates
        self.delphes.Clear()
        array_to_delphes(particles.shape[0], <DTYPE_t*> particles.data,
                         self.pdg, self.delphes, self.all_particles,
                         self.stable_particles, self.partons)
        # run Delphes reconstruction
        self.delphes.ProcessTask()
        candidates = np.empty((self.input_array.GetEntries(),), dtype=dtype_fourvect)
        delphes_to_array(self.input_array, <DTYPE_t*> candidates.data)
        return candidates


@cython.boundscheck(False)
@cython.wraparound(False)
def reconstruct_numpy(DelphesWrapper delphes, np.ndarray particles):
    return delphes.reconstruct_array(particles)


@cython.boundscheck(False)
@cython.wraparound(False)
def reconstruct_hdf5(DelphesWrapper delphes, dataset):
    cdef int num_events = len(dataset)
    cdef np.ndarray particles
    cdef int ievent = 0
    for ievent in range(num_events):
        particles = dataset[ievent]
        yield delphes.reconstruct_array(particles)


@cython.boundscheck(False)
@cython.wraparound(False)
def reconstruct_iterable(DelphesWrapper delphes, iterable, int events):
    cdef np.ndarray particles
    cdef np.ndarray candidates
    cdef int ievent = 0
    for particles in iterable:
        yield delphes.reconstruct_array(particles)
        ievent += 1
        if ievent == events:
            # early termination
            break


@cython.boundscheck(False)
@cython.wraparound(False)
def reconstruct_mc(DelphesWrapper delphes, MCInput gen_input):
    """
    Reconstruct detector-level objects with Delphes from generator input
    """
    while gen_input.get_next_event():
        yield delphes.reconstruct_mc(gen_input)
    gen_input.finish()
