// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC_WRITER_H
#define HEPMC_WRITER_H
///
/// @file  Writer.h
/// @brief Definition of interface \b Writer
///
/// @class HepMC::Writer
/// @brief Base class for all I/O writers
///
/// @ingroup IO
///

#include "HepMC/GenRunInfo.h"

namespace HepMC {

  // Forward declaration
  class GenEvent;

  class Writer {
  public:

    /// Virtual destructor
    virtual ~Writer() {}

    /// Write event @a evt to output target
    virtual void write_event(const GenEvent &evt) = 0;
    virtual bool failed() = 0;
    virtual void close() = 0;

    /// Set the global GenRunInfo object.
    void set_run_info(shared_ptr<GenRunInfo> run) {
      m_run_info = run;
    }

    /// Get the global GenRunInfo object.
    shared_ptr<GenRunInfo> run_info() const {
      return m_run_info;
    }

  private:

    /// The global GenRunInfo object.
    shared_ptr<GenRunInfo> m_run_info;

  };


} // namespace HepMC

#endif
