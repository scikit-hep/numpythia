// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @example hepevt_wrapper_example_main.cc
 *  @brief Basic example of HEPEVT interface use
 *
 *  Fills HEPEVT twice - once using FORTRAN and second time using C++
 *  Tests that both times printouts from FORTRAN and C++ match
 *
 *  Note that HepMC/HEPEVT_Wrapper.h file is a standalone header file
 *  and can be copied over to user directory. No linking to HepMC library
 *  is needed to use this wrapper.
 */


/* These two define statements can be used to change HEPEVT definition.
   They must be defined before including HepMC/HEPEVT_Wrapper.h
   For this test, if these values were to be changed, the same changes
   must be included in FORTRAN code

   NOTE: default is NMXHEP=10000 and double precision */

//#define HEPMC_HEPEVT_NMXHEP 4000
//#define HEPMC_HEPEVT_PRECISION float
#include "HepMC/HEPEVT_Wrapper.h"

#include <iostream>
using HepMC::HEPEVT_Wrapper;
using std::cout;
using std::endl;

extern "C" void simple_tau_hepevt_event_(); //!< Forward declaration of function defined in hepevt_wrapper_example_fortran.f
extern "C" void phodmp_();                  //!< Forward declaration of function defined in hepevt_wrapper_example_fortran.f
extern "C" struct HEPEVT hepevt_;                   //!< Forward declaration of fortran block pointer

/** @brief Add single particle to HEPEVT event */
void add_particle(int id, int status, double px, double py, double pz, double e, double m,
                  int mother1, int mother2, int daughter1, int daughter2) {
    int idx = HEPEVT_Wrapper::number_entries()+1;
    HEPEVT_Wrapper::set_number_entries(idx);

    HEPEVT_Wrapper::set_status(idx, status);
    HEPEVT_Wrapper::set_id(idx, id);
    HEPEVT_Wrapper::set_parents(idx, mother1, mother2);
    HEPEVT_Wrapper::set_children(idx, daughter1, daughter2);
    HEPEVT_Wrapper::set_momentum(idx, px, py, pz ,e);
    HEPEVT_Wrapper::set_mass(idx, m);
}

/**
 * @brief Create simple e+ e- -> Z -> tau+ tau- event
 *
 * In this example we will place the following event into HEPEVT "by hand"
 *
 * 0      Particle Parameters
 * 0 Nr   Type   Parent(s)  Daughter(s)      Px       Py       Pz       E    Inv. M.
 *     1     11      0         3 -    3     0.00    -0.00    45.52    45.52     0.00
 *     2    -11      0         3 -    3    -0.00     0.00   -45.58    45.58     0.00
 *     3     23   1 -    2     4 -    5     0.00     0.00    -0.06    91.11    91.11
 *     4     15      3         6 -    7   -23.19   -26.31   -29.05    45.57     1.78
 *     5    -15      3         8 -    9    23.19    26.31    28.98    45.53     1.78
 *     6     16      4         Stable      -1.26    -1.80    -1.38     2.59     0.01
 *     7   -211      4         Stable     -21.94   -24.51   -27.67    42.98     0.14
 *     8    -16      5         Stable       8.44     8.32     9.62    15.26     0.01
 *     9    211      5         Stable      14.76    17.99    19.36    30.27     0.14
 * 0                       Vector Sum:      0.00     0.00    -0.06    91.11    91.11
 */
void simple_tau_hepevt_event_cpp() {

    HEPEVT_Wrapper::zero_everything();
    HEPEVT_Wrapper::set_event_number(1);

    add_particle(  11, 6,  1.7763568394002505e-15, -3.5565894425761324e-15,  4.5521681043409913e+01, 4.5521681043409934e+01,
                           0.0005111e0,             0,  0,  3,  3);
    add_particle( -11, 6, -1.7763568394002505e-15,  3.5488352204797800e-15, -4.5584999071936601e+01, 4.5584999071936622e+01,
                           0.0005111e0,             0,  0,  3,  3);
    add_particle(  23, 5,  0e0,                     0e0,                    -6.3318028526687442e-02, 9.1106680115346506e+01,
                           9.1106658112716090e+01,  1,  2,  4,  5);
    add_particle(  15, 2, -2.3191595992562256e+01, -2.6310500920665142e+01, -2.9046412466624929e+01, 4.5573504956498098e+01,
                           1.7769900000002097e+00,  3,  0,  6,  7);
    add_particle( -15, 2,  2.3191595992562256e+01,  2.6310500920665142e+01,  2.8983094438098242e+01, 4.5533175158848429e+01,
                           1.7769900000000818e+00,  3,  0,  8,  9);
    add_particle(  16, 1, -1.2566536214715378e+00, -1.7970251138317268e+00, -1.3801323581022720e+00, 2.5910119010468553e+00,
                           9.9872238934040070e-03,  4,  0,  0, 0);
    add_particle(-211, 1, -2.1935073012334062e+01, -2.4513624017269400e+01, -2.7666443730700312e+01, 4.2982749776866747e+01,
                           1.3956783711910248e-01,  4,  0,  0, 0);
    add_particle( -16, 1,  8.4364531743909055e+00,  8.3202830831667836e+00,  9.6202800273055971e+00, 1.5262723881157640e+01,
                           9.9829332903027534e-03,  5,  0,  0, 0);
    add_particle( 211, 1,  1.4755273459419701e+01,  1.7990366047940022e+01,  1.9362977676297948e+01, 3.0270707771933196e+01,
                           1.3956753909587860e-01,  5,  0,  0, 0);
}

/** Main program */
int main() {
    cout << endl << "HEPEVT wrapper example - FORTRAN EVENT" << endl;
    cout <<         "--------------------------------------" << endl;

    HEPEVT_Wrapper::set_hepevt_address((char*)(&hepevt_));

    simple_tau_hepevt_event_();

    cout << endl << "FORTRAN PRINTOUT" << endl << endl;
    phodmp_();

    cout << endl << "C++ PRINTOUT"     << endl << endl;
    HEPEVT_Wrapper::print_hepevt();

    cout << endl << "HEPEVT wrapper example - C++ EVENT" << endl;
    cout <<         "----------------------------------" << endl;

    simple_tau_hepevt_event_cpp();

    cout << endl << "FORTRAN PRINTOUT" << endl << endl;
    phodmp_();

    cout << endl << "C++ PRINTOUT" << endl << endl;
    HEPEVT_Wrapper::print_hepevt();
}
