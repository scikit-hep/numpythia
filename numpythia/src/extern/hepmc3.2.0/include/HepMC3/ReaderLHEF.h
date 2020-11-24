// -*- C++ -*-
// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_READERLHEF_H
#define HEPMC3_READERLHEF_H
/**
 *  @file  ReaderLHEF.h
 *  @brief Definition of \b class ReaderLHEF
 *
 *  @class HepMC3::ReaderLHEF
 *  @brief GenEvent I/O parsing and serialization for LHEF files
 *
 *
 *  @ingroup IO
 *
 */
#include "HepMC3/Reader.h"
#include "HepMC3/GenEvent.h"
#include <string>
#include <fstream>
#include <istream>
#include "HepMC3/LHEFAttributes.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include <iomanip>


namespace HepMC3
{
class ReaderLHEF : public Reader
{
public:
#ifndef HEPMC3_PYTHON_BINDINGS
    /// The ctor to read from stream
    ReaderLHEF(std::istream &);
#endif
private:
    void init();                       ///< Init helper
public:
    /** @brief Constructor */
    ReaderLHEF(const std::string& filename);
    /// @brief skip events
    bool skip(const int)  override;
    /** @brief Reading event */
    bool read_event(GenEvent& ev)  override;
    /** @brief Close */
    void close()  override;
    /** @brief State */
    bool failed()  override;
    /** @brief Destructor */
    ~ReaderLHEF() ;
private:
    LHEF::Reader* m_reader;            ///< The actual reader
    shared_ptr<HEPRUPAttribute> m_hepr; ///< Holder of attributes
    int m_neve;                         ///< Event counter
    bool m_failed;                      ///< State of reader
};
}
#endif
