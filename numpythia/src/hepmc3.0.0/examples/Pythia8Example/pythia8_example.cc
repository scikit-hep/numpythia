/**
 *  @example pythia8_example.cc
 *  @brief Basic example of use for pythia8 interface
 *
 */
#include "HepMC/GenEvent.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/Print.h"

#include "Pythia8/Pythia.h"
#include "Pythia8/Pythia8ToHepMC3.h"

#include <iostream>
using namespace HepMC;

/** Main program */
int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <pythia_config_file> <output_hepmc3_file>" << endl;
        exit(-1);
    }

    Pythia8::Pythia pythia;
    Pythia8ToHepMC3 pythiaToHepMC;
    pythia.readFile(argv[1]);
    pythia.init();

    WriterAscii file(argv[2]);

    int nEvent = pythia.mode("Main:numberOfEvents");

    for( int i = 0; i< nEvent; ++i ) {
        if( !pythia.next() ) continue;

        HepMC::GenEvent hepmc( Units::GEV, Units::MM );

        pythiaToHepMC.fill_next_event(pythia.event, &hepmc, -1, &pythia.info);

        if( i==0 ) {
            std::cout << "First event: " << std::endl;
            Print::listing(hepmc);
        }

        file.write_event(hepmc);
    }

    file.close();
    pythia.stat();
}
