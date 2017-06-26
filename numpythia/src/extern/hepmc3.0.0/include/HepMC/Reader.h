// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC_READER_H
#define HEPMC_READER_H
///
/// @file  Reader.h
/// @brief Definition of interface \b Reader
///
/// @class HepMC::Reader
/// @brief Base class for all I/O readers
///
/// @ingroup IO
///

#include "HepMC/GenRunInfo.h"

namespace HepMC {

  // Forward declaration
  class GenEvent;

  class Reader {
  public:

    /// Virtual destructor
    virtual ~Reader() {}

    /// Fill next event from input into @a evt
    virtual bool read_event(GenEvent& evt) = 0;
    virtual bool failed()=0;
    virtual void close()=0;

    /// Get the global GenRunInfo object.
    shared_ptr<GenRunInfo> run_info() const {
      return m_run_info;
    }

  protected:

    /// Set the global GenRunInfo object.
    void set_run_info(shared_ptr<GenRunInfo> run) {
      m_run_info = run;
    }

  private:

    /// The global GenRunInfo object.
    shared_ptr<GenRunInfo> m_run_info;

  };


} // namespace HepMC

#endif
