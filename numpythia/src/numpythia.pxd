from libcpp.vector cimport vector
from libcpp.string cimport string

cimport hepmc as HepMC
cimport pythia as Pythia

cdef extern from "numpythia.h":
    #void hepmc_to_pseudojet(GenEvent&, vector[PseudoJet]&, double)
    #void pythia_to_pseudojet(Event&, vector[PseudoJet]&, double)
    HepMC.GenEvent* pythia_to_hepmc(Pythia.Pythia*, HepMC.MomentumUnit, HepMC.LengthUnit)
    void hepmc_finalstate_particles(HepMC.GenEvent*, vector[HepMC.GenParticle*]&)
    void hepmc_to_array(vector[HepMC.GenParticle*]& particles, double*)

    # Delphes (optional)
    #void array_to_delphes(int num_particles, double* particles, TDatabasePDG* pdg,
                          #Delphes* delphes, TObjArray* all_particles,
                          #TObjArray* stable_particles, TObjArray* partons) 
    #void pythia_to_delphes(Event&, Delphes*, TObjArray*, TObjArray*, TObjArray*)
    #void hepmc_to_delphes(GenEvent* event, TDatabasePDG* pdg,
                          #Delphes* delphes, TObjArray* all_particles,
                          #TObjArray* stable_particles, TObjArray* partons) 
    #void delphes_to_pseudojet(TObjArray*, vector[PseudoJet]&)
    #void delphes_to_array(TObjArray* input_array, double* array)
