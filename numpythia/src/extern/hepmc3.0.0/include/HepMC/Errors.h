// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)

#ifndef HEPMC_ERRORS_H
#define HEPMC_ERRORS_H

#include "HepMC/Common.h"

namespace HepMC {


    /// @name Printing macros
    //@{

    /** @brief Macro for printing error messages */
    #define ERROR(MESSAGE)   if ( Setup::print_errors() )   { std::cerr << "ERROR::" << MESSAGE << std::endl; }

    /** @brief Macro for printing warning messages */
    #define WARNING(MESSAGE) if ( Setup::print_warnings() ) { std::cout << "WARNING::" << MESSAGE << std::endl; }

    // Debug messages and code that will not go to the release version
    #ifndef HEPMC_RELEASE_VERSION

    /** @brief Macro for printing debug messages with appropriate debug level */
    #define DEBUG(LEVEL,MESSAGE) if( Setup::debug_level()>=(LEVEL) ) { std::cout << "DEBUG(" << LEVEL <<")::" << MESSAGE << std::endl; }
    /** @brief Macro for storing code useful for debugging */
    #define DEBUG_CODE_BLOCK( x ) x

    #else

    #define DEBUG( x,y )
    #define DEBUG_CODE_BLOCK( x )

    #endif

    //@}


    /// @name Exceptions
    /// @todo Move to a dedicated header once we have more than one
    //@{

    /// @class Exception
    /// @brief Standard runtime error
    /// @todo Should it be GenException? It's a HepMC thing, but not really a "gen" thing
    struct Exception : public std::runtime_error {
      Exception(const std::string& msg) : std::runtime_error(msg) {} ///< Default constructor
    };

    /// @brief Exception related to weight lookups, setting, and index consistency
    struct WeightError : public Exception {
      WeightError(const std::string& msg) : Exception(msg) {} ///< Default constructor
    };

    //@}


} // namespace HepMC

#endif
