#include "Pythia8/Pythia.h"

#include "HepMC/Common.h"
#include "HepMC/Data/SmartPointer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/Pythia8ToHepMC3.h"

//#include "fastjet/ClusterSequence.hh"

//#include "Delphes/modules/Delphes.h"
//#include "Delphes/classes/DelphesClasses.h"
//#include "Delphes/classes/DelphesFactory.h"

//#include "TObjArray.h"
//#include "TLorentzVector.h"
//#include "TDatabasePDG.h"
//#include "TParticlePDG.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <cmath>
//#include <algorithm>
//#include <math.h>
#include <vector>


HepMC::GenEvent* pythia_to_hepmc(Pythia8::Pythia* pythia) {
    HepMC::Pythia8ToHepMC3 py2hepmc;
    // Suppress warnings
    py2hepmc.set_print_inconsistency(false);
    HepMC::GenEvent* event = new HepMC::GenEvent(HepMC::Units::GEV, HepMC::Units::MM);
    if (!py2hepmc.fill_next_event(*pythia, event)) {
        delete event;
        return NULL;
    }
    return event;
}


void hepmc_to_array(std::vector<HepMC::SmartPointer<HepMC::GenParticle> >& particles,
                    char* array, unsigned int rowbytes) {
    HepMC::FourVector momentum, prod_vertex;
    char* row;
    double* double_fields;
    int* int_fields;
    unsigned int i = 0;
    FOREACH (const HepMC::SmartPointer<HepMC::GenParticle>& particle, particles) {
        momentum = particle->momentum();
        prod_vertex = particle->production_vertex()->position();
        row = &array[i * rowbytes];
        // doubles
        double_fields = (double*) row;
        double_fields[0] = momentum.e();
        double_fields[1] = momentum.px();
        double_fields[2] = momentum.py();
        double_fields[3] = momentum.pz();
        double_fields[4] = momentum.m();
        double_fields[5] = prod_vertex.x();
        double_fields[6] = prod_vertex.y();
        double_fields[7] = prod_vertex.z();
        double_fields[8] = prod_vertex.t();
        // integers
        int_fields = (int*)&row[9 * sizeof(double)];
        int_fields[0] = particle->pid();
        int_fields[1] = particle->status();
        ++i;
    }
}

/*void hepmc_to_pseudojet(HepMC::GenEvent& evt, std::vector<fastjet::PseudoJet>& output, double eta_max) {*/
  //int pdgid;
  //HepMC_IsStateFinal isfinal;
  //HepMC::FourVector fourvect;
  //output.clear();
  //for (HepMC::GenEvent::particle_iterator p = evt.particles_begin(); p != evt.particles_end(); ++p) if (isfinal(*p)) {
    //// visibility test
    //pdgid = abs((*p)->pdg_id());
    //if ((pdgid == 12) || (pdgid == 14) || (pdgid == 16)) continue; // neutrino
    //fourvect = (*p)->momentum();
    //if (abs(fourvect.pseudoRapidity()) > eta_max) continue;
    //fastjet::PseudoJet particleTemp(fourvect.px(), fourvect.py(), fourvect.pz(), fourvect.e());
    //output.push_back(particleTemp);
  //}
//}


//void pythia_to_pseudojet(Pythia8::Event& event, std::vector<fastjet::PseudoJet>& output, double eta_max) {
  //output.clear();
  //for (int i = 0; i < event.size(); ++i) if (event[i].isFinal()) {
    //// Require visible particles inside detector.
    //if (!event[i].isVisible()) continue;
    //if (abs(event[i].eta()) > eta_max) continue;
    //// Create a PseudoJet from the complete Pythia particle.
    //fastjet::PseudoJet particleTemp = event[i];
    //// TODO: associate particle to pseudojet with
    //// fastjet::PseudoJet::UserInfoBase
    //// see
    //// http://fastjet.hepforge.org/svn/contrib/contribs/VertexJets/trunk/example.cc
    //output.push_back(particleTemp);
  //}
//}


//void pythia_to_delphes(Pythia8::Event& event, Delphes* delphes,
                       //TObjArray* all_particles,
                       //TObjArray* stable_particles,
                       //TObjArray* partons) {
    //// Based on code here:
    //// https://cp3.irmp.ucl.ac.be/projects/delphes/browser/examples/ExternalFastJet/ExternalFastJetBasic.cpp
    //DelphesFactory* factory = delphes->GetFactory();
    //Candidate* candidate;
    //int pdgid;
    //for (int i = 0; i < event.size(); ++i) {
        //if (!event[i].isVisible()) continue;
        //candidate = factory->NewCandidate();
        //const Pythia8::Particle& particle = event[i];
        //candidate->PID = particle.id();
        //pdgid = particle.idAbs();
        //candidate->Status = particle.status();
        //candidate->Charge = particle.charge();
        //candidate->Mass = particle.m();
        //candidate->Momentum.SetPxPyPzE(particle.px(), particle.py(), particle.pz(), particle.e());
        //candidate->Position.SetXYZT(particle.xProd(), particle.yProd(), particle.zProd(), particle.tProd());
        //all_particles->Add(candidate);
        //if (particle.isFinal()) {
            //stable_particles->Add(candidate);
        //} else if (pdgid <= 5 || pdgid == 21 || pdgid == 15) {
            //partons->Add(candidate);
        //}
    //}
//}


//void hepmc_to_delphes(HepMC::GenEvent* event, TDatabasePDG* pdg,
                      //Delphes* delphes, TObjArray* all_particles,
                      //TObjArray* stable_particles, TObjArray* partons) {
    //DelphesFactory* factory = delphes->GetFactory();
    //Candidate* candidate;
    //HepMC::GenParticle* particle;
    //HepMC::FourVector momentum, prod_vertex;
    //HepMC_IsStateFinal isfinal;
    //TParticlePDG *pdgParticle;
    //int pdgid;
    //for (HepMC::GenEvent::particle_iterator p = event->particles_begin(); p != event->particles_end(); ++p) {
        //particle = *p;
        //pdgid = abs(particle->pdg_id());
        //if ((pdgid == 12) || (pdgid == 14) || (pdgid == 16)) continue; // neutrino
        //pdgParticle = pdg->GetParticle(particle->pdg_id());
        //momentum = particle->momentum();
        //candidate = factory->NewCandidate();
        //candidate->PID = particle->pdg_id();
        //candidate->Status = particle->status();
        //candidate->Charge = pdgParticle ? int(pdgParticle->Charge()/3.0) : -999;
        //candidate->Mass = momentum.m();
        //candidate->Momentum.SetPxPyPzE(momentum.px(), momentum.py(), momentum.pz(), momentum.e());
        //if (particle->production_vertex() == NULL) {
            //candidate->Position.SetXYZT(0, 0, 0, 0);
        //} else {
            //prod_vertex = particle->production_vertex()->position();
            //candidate->Position.SetXYZT(prod_vertex.x(), prod_vertex.y(), prod_vertex.z(), prod_vertex.t());
        //}
        //all_particles->Add(candidate);
        //if (isfinal(particle)) {
            //stable_particles->Add(candidate);
        //} else if (pdgid <= 5 || pdgid == 21 || pdgid == 15) {
            //partons->Add(candidate);
        //}
    //}
//}


//void array_to_delphes(int num_particles, double* particles, TDatabasePDG* pdg,
                      //Delphes* delphes, TObjArray* all_particles,
                      //TObjArray* stable_particles, TObjArray* partons) {
    //// Only stable particles
    //DelphesFactory* factory = delphes->GetFactory();
    //Candidate* candidate;
    //TParticlePDG *pdgParticle;
    //double* particle;
    //for (int iparticle = 0; iparticle < num_particles; ++iparticle) {
        //particle = &particles[iparticle * 10];
        //pdgParticle = pdg->GetParticle(particle[9]);
        //candidate = factory->NewCandidate();
        //candidate->PID = particle[9];
        //candidate->Status = 1;
        //candidate->Charge = pdgParticle ? int(pdgParticle->Charge()/3.0) : -999;
        //candidate->Mass = particle[4];
        //candidate->Momentum.SetPxPyPzE(particle[1], particle[2], particle[3], particle[0]);
        //candidate->Position.SetXYZT(particle[5], particle[6], particle[7], particle[8]);
        //all_particles->Add(candidate);
        //stable_particles->Add(candidate);
    //}
//}


//void delphes_to_pseudojet(TObjArray* input_array, std::vector<fastjet::PseudoJet>& output) {
    //// Based on code here:
    //// https://cp3.irmp.ucl.ac.be/projects/delphes/browser/examples/ExternalFastJet/ExternalFastJetBasic.cpp
    //output.clear();
    //TIterator* input_iterator = input_array->MakeIterator();
    //Candidate* candidate;
    //fastjet::PseudoJet pseudojet;
    //TLorentzVector momentum;
    //while((candidate = static_cast<Candidate*>(input_iterator->Next()))) {
        //momentum = candidate->Momentum;
        //pseudojet = fastjet::PseudoJet(momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E());
        //// TODO: associate Candidate to PseudoJet with a
        //// fastjet::PseudoJet::UserInfoBase
        //// see
        //// http://fastjet.hepforge.org/svn/contrib/contribs/VertexJets/trunk/example.cc
        //output.push_back(pseudojet);
    //}
//}


//void delphes_to_array(TObjArray* input_array, double* array) {
    //TIterator* input_iterator = input_array->MakeIterator();
    //Candidate* candidate;
    //TLorentzVector momentum;
    //unsigned int icand = 0;
    //while((candidate = static_cast<Candidate*>(input_iterator->Next()))) {
        //momentum = candidate->Momentum;
        //array[icand * 4 + 0] = momentum.E();
        //array[icand * 4 + 1] = momentum.Px();
        //array[icand * 4 + 2] = momentum.Py();
        //array[icand * 4 + 3] = momentum.Pz();
        //++icand;
    //}
/*}*/
