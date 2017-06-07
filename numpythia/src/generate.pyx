import os


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

    cdef GenEvent* get_hepmc(self):
        pass

    cdef void to_pseudojet(self, vector[PseudoJet]& particles, float eta_max):
        pass

    cdef void to_delphes(self, Delphes* modular_delphes,
                         TObjArray* delphes_all_particles,
                         TObjArray* delphes_stable_particles,
                         TObjArray* delphes_partons):
        pass

    cdef void finish(self):
        pass


cdef class PythiaInput(MCInput):

    cdef Pythia* pythia
    cdef VinciaPlugin* vincia_plugin
    cdef UserHooks* userhooks
    cdef GenEvent* hepmc_event
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

        self.pythia = new Pythia(xmldoc, False)

        # Initialize pointers to NULL
        self.vincia_plugin = NULL
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
        if shower == 'vincia':
            self.vincia_plugin = new VinciaPlugin(self.pythia, config)

        else:  # default Pythia shower
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

        if shower == 'vincia':
            # vincia calls pythia's init
            # but we lose the bool return value of Pythia's init
            # if init fails there, pythia.next() will anyway abort
            # TODO: follow this up with Skands et al
            success = self.vincia_plugin.init()
        else:
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
        del self.vincia_plugin
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
        cdef int nweights = self.pythia.info.nWeights()
        if nweights == 1:
            # only nominal is present so don't bother with weights since it is
            # normally 1. DIRE does produce weighted events, however.
            return 0
        return nweights

    cdef bool get_next_event(self) except *:
        # generate event and quit if failure
        if not self.pythia.next():
            raise RuntimeError("PYTHIA event generation aborted prematurely")
        if self.num_weights > 0:
            self.weights = np.empty(self.num_weights, dtype=DTYPE)
            for iweight in range(self.num_weights):
                self.weights[iweight] = self.pythia.info.weight(iweight)
        if not keep_pythia_event(self.pythia.event, self.cut_on_pdgid,
                                 self.pdgid_pt_min, self.pdgid_pt_max):
            # event doesn't pass our truth-level cuts
            return False
        return True

    cdef GenEvent* get_hepmc(self):
        del self.hepmc_event
        self.hepmc_event = pythia_to_hepmc(self.pythia)
        return self.hepmc_event

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

    cdef void finish(self):
        if self.verbosity > 0:
            self.pythia.stat()


cdef class HepMCInput(MCInput):

    cdef string filename
    cdef IO_GenEvent* hepmc_reader
    cdef GenEvent* event
    cdef TDatabasePDG *pdg

    def __cinit__(self, string filename):
        self.filename = filename
        self.hepmc_reader = get_hepmc_reader(filename)
        self.event = NULL
        self.pdg = TDatabasePDG_Instance()

    def __dealloc__(self):
        del self.event
        del self.hepmc_reader

    cdef bool get_next_event(self) except *:
        del self.event
        self.event = self.hepmc_reader.read_next_event()
        if self.event == NULL:
            return False
        return True

    cdef GenEvent* get_hepmc(self):
        return self.event

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
def generate_events(MCInput gen_input, int n_events, string write_to, bool ignore_weights=False):
    """
    Generate events (or read HepMC) and yield numpy arrays of particles
    If weights are enabled, this function will yield the particles and weights array
    """
    cdef np.ndarray particle_array
    cdef GenEvent* event
    cdef IO_GenEvent* hepmc_writer = NULL
    cdef vector[GenParticle*] particles
    cdef int ievent = 0;
    cdef bool weighted = gen_input.get_num_weights() > 0
    if n_events < 0:
        ievent = n_events - 1
    if not write_to.empty():
        hepmc_writer = get_hepmc_writer(write_to)
    while ievent < n_events:
        if not gen_input.get_next_event():
            continue
        # We don't own event here. MCInput will delete it.
        event = gen_input.get_hepmc()
        if hepmc_writer != NULL:
            hepmc_writer.write_event(event)
        hepmc_finalstate_particles(event, particles)
        particle_array = np.empty((particles.size(),), dtype=dtype_particle)
        particles_to_array(particles, <DTYPE_t*> particle_array.data)
        if weighted and not ignore_weights:
            yield particle_array, gen_input.weights
        else:
            yield particle_array
        if n_events > 0:
            ievent += 1
    gen_input.finish()
    del hepmc_writer
