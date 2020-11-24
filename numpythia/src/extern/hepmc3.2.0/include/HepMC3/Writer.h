// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_WRITER_H
#define HEPMC3_WRITER_H
///
/// @file  Writer.h
/// @brief Definition of interface \b Writer
///
/// @class HepMC3::Writer
/// @brief Base class for all I/O writers
///
/// @ingroup IO
///

#include "HepMC3/GenRunInfo.h"

namespace HepMC3 {

// Forward declaration
class GenEvent;

class Writer {
public:

    ///Constructor
    Writer() {}
    
    /// Virtual destructor
    virtual ~Writer() {}

    /// Write event @a evt to output target
    virtual void write_event(const GenEvent &evt) = 0;
    /** @brief Get file and/or stream error state */
    virtual bool failed() = 0;
    /** @brief Close file and/or stream */
    virtual void close() = 0;

    /// Set the global GenRunInfo object.
    void set_run_info(shared_ptr<GenRunInfo> run) {
        m_run_info = run;
    }

    /// Get the global GenRunInfo object.
    shared_ptr<GenRunInfo> run_info() const {
        return m_run_info;
    }

///deleted copy constructor
    Writer(const Writer&) = delete; 
///deleted copy assignment operator
    Writer& operator = (const Writer &) = delete;            
    /// Set options
    void set_options(const std::map<std::string, std::string>& options)
    {
    m_options=options;
    }
    /// Set options
    std::map<std::string, std::string> get_options() const
    {
    return m_options;
    }
protected:

    /// options
    std::map<std::string, std::string> m_options;
   
private:

    /// The global GenRunInfo object.
    shared_ptr<GenRunInfo> m_run_info;
 
};


} // namespace HepMC3

#endif
