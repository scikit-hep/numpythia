// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef Pythia8_Pythia8ToHepMC3_H
#define Pythia8_Pythia8ToHepMC3_H

#include "Pythia8/Pythia.h"

#include <vector>
namespace HepMC {

class GenEvent;
class GenVertex;
class GenParticle;

class Pythia8ToHepMC3 {

public:

    // Constructor and destructor
    Pythia8ToHepMC3(): m_internal_event_number(0),
                       m_print_inconsistency(true),
                       m_free_parton_warnings(true),
                       m_crash_on_problem(false),
                       m_convert_gluon_to_0(false),
                       m_store_pdf(true),
                       m_store_proc(true),
                       m_store_xsec(true) {}

    virtual ~Pythia8ToHepMC3() {}

    // The recommended method to convert Pythia events into HepMC ones
    bool fill_next_event( Pythia8::Pythia& pythia, GenEvent* evt, int ievnum = -1 )
    { return fill_next_event( pythia.event, evt, ievnum, &pythia.info, &pythia.settings); }

    // Alternative method to convert Pythia events into HepMC ones
    bool fill_next_event( Pythia8::Event& pyev, GenEvent* evt,
                          int ievnum = -1, Pythia8::Info* pyinfo = 0,
                          Pythia8::Settings* pyset = 0);

    // Read out values for some switches
    bool print_inconsistency()  const { return m_print_inconsistency;  }
    bool free_parton_warnings() const { return m_free_parton_warnings; }
    bool crash_on_problem()     const { return m_crash_on_problem;     }
    bool convert_gluon_to_0()   const { return m_convert_gluon_to_0;   }
    bool store_pdf()            const { return m_store_pdf;            }
    bool store_proc()           const { return m_store_proc;           }
    bool store_xsec()           const { return m_store_xsec;           }

    // Set values for some switches
    void set_print_inconsistency(bool b = true)  { m_print_inconsistency  = b; }
    void set_free_parton_warnings(bool b = true) { m_free_parton_warnings = b; }
    void set_crash_on_problem(bool b = false)    { m_crash_on_problem     = b; }
    void set_convert_gluon_to_0(bool b = false)  { m_convert_gluon_to_0   = b; }
    void set_store_pdf(bool b = true)            { m_store_pdf            = b; }
    void set_store_proc(bool b = true)           { m_store_proc           = b; }
    void set_store_xsec(bool b = true)           { m_store_xsec           = b; }

private:

    // Following methods are not implemented for this class
    virtual bool fill_next_event( GenEvent*  )  { return 0; }
    virtual void write_event( const GenEvent* ) {}

    // Use of copy constructor is not allowed
    Pythia8ToHepMC3( const Pythia8ToHepMC3& ) {}

    // Data members
    int  m_internal_event_number;
    bool m_print_inconsistency;
    bool m_free_parton_warnings;
    bool m_crash_on_problem;
    bool m_convert_gluon_to_0;
    bool m_store_pdf;
    bool m_store_proc;
    bool m_store_xsec;
};

} // namespace HepMC
#endif
