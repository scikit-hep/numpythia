// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_CROSS_SECTION_H
#define  HEPMC_CROSS_SECTION_H
/**
 *  @file GenCrossSection.h
 *  @brief Definition of attribute \b class GenCrossSection
 *
 *  @class HepMC::GenCrossSection
 *  @brief Stores additional information about cross-section
 *
 *  This is an example of event attribute used to store cross-section information
 *
 *  This class is meant to be used to pass, on an event by event basis,
 *  the current best guess of the total cross section.
 *  It is expected that the final cross section will be stored elsewhere.
 *
 *    - double cross_section;           // cross section in pb
 *    - double cross_section_error;     // error associated with this cross section
 *
 *  The units of cross_section and cross_section_error are expected to be pb.
 *
 *  @ingroup attributes
 *
 */
#include <iostream>
#include "HepMC/Attribute.h"

namespace HepMC {


class GenCrossSection : public Attribute {

//
// Fields
//
public:

    double cross_section;       ///< Generated cross-section
    double cross_section_error; ///< Generated cross-section error
    long accepted_events;       ///< The number of events generated so far.
    long attempted_events;      ///< The number of events attempted so far.

//
// Functions
//
public:
    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att);

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const;

    /** @brief Set all fields */
  void set_cross_section( double xs, double xs_err, long n_acc = -1, long n_att = -1) {
        cross_section       = xs;
        cross_section_error = xs_err;
	accepted_events     = n_acc;
	attempted_events    = n_att;
    }

    bool operator==( const GenCrossSection& ) const; ///< Operator ==
    bool operator!=( const GenCrossSection& ) const; ///< Operator !=
    bool is_valid()                           const; ///< Verify that the instance contains non-zero information
};


} // namespace HepMC

#endif
