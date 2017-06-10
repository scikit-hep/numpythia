// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/// @file Setup.h
/// @brief Definition of \b class Setup

#ifndef HEPMC_SETUP_H
#define HEPMC_SETUP_H

// #include <iostream>

namespace HepMC {


    /// @todo Move some of the helper functions, logging, exceptions, etc. to specific files


    /// @brief Configuration for HepMC
    ///
    /// Contains macro definitions for printing debug output, feature deprecation, etc.
    /// Static class - configuration is shared among all HepMC events
    /// and program threads
    ///
    /// @todo Change to a namespace
    class Setup {

        /// Private constructor
        Setup() {}
        /// Private destructor
        ~Setup() {}


    public:

        /// @name Accessors
        //@{

        /// Get error messages printing flag
        static bool print_errors()                { return m_is_printing_errors;    }
        /// set error messages printing flag
        static void set_print_errors(bool flag)   { m_is_printing_errors   = flag;  }

        /// Get warning messages printing flag
        static bool print_warnings()              { return m_is_printing_warnings;  }
        /// Set warning messages printing flag
        static void set_print_warnings(bool flag) { m_is_printing_warnings = flag;  }

        /// Get debug level
        static int  debug_level()                 { return m_debug_level;           }
        /// Set debug level
        static void set_debug_level(int level)    { m_debug_level          = level; }
        //@}

        /// @name Static constants
        //@{
        /// Default maxUlps for AlmostEqual2sComplement function (double precision)
        static const unsigned int DEFAULT_DOUBLE_ALMOST_EQUAL_MAXULPS;

        /// Default threshold for comparing double variables
        static const double DOUBLE_EPSILON;

        //@}


    private:

        static bool m_is_printing_errors;   //!< Flag for printing error messages
        static bool m_is_printing_warnings; //!< Flag for printing warning messages
        static int  m_debug_level;          //!< Level of debug messages printed out
    };


} // namespace HepMC

#endif
