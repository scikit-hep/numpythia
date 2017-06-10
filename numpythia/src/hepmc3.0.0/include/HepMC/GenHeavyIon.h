// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_HEAVYION_H
#define  HEPMC_HEAVYION_H
/**
 *  @file GenHeavyIon.h
 *  @brief Definition of attribute \b class GenHeavyIon
 *
 *  @class HepMC::GenHeavyIon
 *  @brief Stores additional information about Heavy Ion generator
 *
 *  This is an example of event attribute used to store Heavy Ion information
 *
 *  @ingroup attributes
 *
 */
#include <iostream>
#include "HepMC/Attribute.h"

namespace HepMC {


class GenHeavyIon : public Attribute {

//
// Fields
//
public:
    int    Ncoll_hard;                   ///< Number of hard collisions
    int    Npart_proj;                   ///< Number of participating nucleons in the projectile
    int    Npart_targ;                   ///< Number of participating nucleons in the target
    int    Ncoll;                        ///< Number of collisions
    int    spectator_neutrons;           ///< Number of spectator neutrons
    int    spectator_protons;            ///< Number of spectator protons
    int    N_Nwounded_collisions;        ///< @todo Describe!
    int    Nwounded_N_collisions;        ///< @todo Describe!
    int    Nwounded_Nwounded_collisions; ///< @todo Describe!
    double impact_parameter;             ///< Impact parameter
    double event_plane_angle;            ///< Event plane angle
    double eccentricity;                 ///< Eccentricity
    double sigma_inel_NN;                ///< Assumed nucleon-nucleon cross-section
    double centrality;                   ///< Centrality

//
// Functions
//
public:
    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att);

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const;

    /** @brief Set all fields */
    void set( int nh, int np, int nt, int nc, int ns, int nsp,
              int nnw=0, int nwn=0, int nwnw=0,
              float im=0., float pl=0., float ec=0., float s=0., float cent=0. );

    bool operator==( const GenHeavyIon& ) const; ///< Operator ==
    bool operator!=( const GenHeavyIon& ) const; ///< Operator !=
    bool is_valid()                       const; ///< Verify that the instance contains non-zero information
};


#ifndef HEPMC_NO_DEPRECATED
typedef GenHeavyIon HeavyIon; ///< Backward compatibility typedef
#endif


} // namespace HepMC

#endif
