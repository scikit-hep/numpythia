
cdef extern from "numpythia.h":
    IO_GenEvent* get_hepmc_reader(string)
    IO_GenEvent* get_hepmc_writer(string)
    void hepmc_to_pseudojet(GenEvent&, vector[PseudoJet]&, double)
    void pythia_to_pseudojet(Event&, vector[PseudoJet]&, double)
    GenEvent* pythia_to_hepmc(Pythia*)
    void hepmc_finalstate_particles(GenEvent*, vector[GenParticle*]&)
    void hepmc_to_array(vector[GenParticle*]& particles, double*)

    # Delphes (optional)
    void array_to_delphes(int num_particles, double* particles, TDatabasePDG* pdg,
                          Delphes* delphes, TObjArray* all_particles,
                          TObjArray* stable_particles, TObjArray* partons) 
    void pythia_to_delphes(Event&, Delphes*, TObjArray*, TObjArray*, TObjArray*)
    void hepmc_to_delphes(GenEvent* event, TDatabasePDG* pdg,
                          Delphes* delphes, TObjArray* all_particles,
                          TObjArray* stable_particles, TObjArray* partons) 
    void delphes_to_pseudojet(TObjArray*, vector[PseudoJet]&)
    void delphes_to_array(TObjArray* input_array, double* array)
