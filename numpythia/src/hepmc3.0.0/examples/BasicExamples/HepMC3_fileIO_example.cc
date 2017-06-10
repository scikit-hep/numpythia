// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @example HepMC3_fileIO_example.cc
 *  @brief Test of file I/O
 *
 *  Parses HepMC3 file and saves it as a new HepMC3 file.
 *  The resulting file should be an exact copy of the input file
 *
 */
#include "HepMC/GenEvent.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/Print.h"

#include <iostream>
using namespace HepMC;
using std::cout;
using std::endl;

/** Main program */
int main(int argc, char **argv) {

    if( argc<3 ) {
        cout << "Usage: " << argv[0] << " <HepMC3_input_file> <output_file>" << endl;
        exit(-1);
    }

    ReaderAscii input_file (argv[1]);
    WriterAscii output_file(argv[2]);

    int events_parsed = 0;

    while(!input_file.failed()) {
        GenEvent evt(Units::GEV,Units::MM);

        // Read event from input file
        input_file.read_event(evt);

        // If reading failed - exit loop
        if( input_file.failed() ) break;

        // Save event to output file
        output_file.write_event(evt);

        if(events_parsed==0) {
            cout << " First event: " << endl;
            Print::listing(evt);
            Print::content(evt);

            cout << " Testing attribute reading for the first event: " << endl;

            shared_ptr<GenCrossSection> cs = evt.attribute<GenCrossSection>("GenCrossSection");
            shared_ptr<GenHeavyIon>     hi = evt.attribute<GenHeavyIon>("GenHeavyIon");
            shared_ptr<GenPdfInfo>      pi = evt.attribute<GenPdfInfo>("GenPdfInfo");

            if(cs) {
                cout << " Has GenCrossSection:   ";
                Print::line(cs);
            }
            else cout << " No GenCrossSection " << endl;

            if(pi) {
                cout << " Has GenPdfInfo:        ";
                Print::line(pi);
            }
            else cout << " No GenPdfInfo " << endl;

            if(hi) {
                cout << " Has GenHeavyIon:       ";
                Print::line(hi);
            }
            else cout << " No GenHeavyIon " << endl;
        }

        ++events_parsed;
        if( events_parsed%100 == 0 ) cout<<"Events parsed: "<<events_parsed<<endl;
    }

    input_file.close();
    output_file.close();

    return 0;
}
