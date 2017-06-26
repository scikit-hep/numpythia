// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file GenHeavyIon.cc
 *  @brief Implementation of \b class GenHeavyIon
 *
 */
#include "HepMC/GenHeavyIon.h"
#include <cstring> // memcmp
#include <cstdlib> // atoi
#include <cstdio> // sprintf

namespace HepMC {

bool GenHeavyIon::from_string(const string &att) {
    const char *cursor = att.data();

    Ncoll_hard = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    Npart_proj = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    Npart_targ = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    Ncoll = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    spectator_neutrons = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    spectator_protons = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    N_Nwounded_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    Nwounded_N_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    Nwounded_Nwounded_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    impact_parameter = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    event_plane_angle = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    eccentricity = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    sigma_inel_NN = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    centrality = atof(cursor);

    return true;
}

bool GenHeavyIon::to_string(string &att) const {
    char buf[255];

    sprintf(buf,"%i %i %i %i %i %i %i %i %i %.8e %.8e %.8e %.8e %.8e",
            Ncoll_hard,
            Npart_proj,
            Npart_targ,
            Ncoll,
            spectator_neutrons,
            spectator_protons,
            N_Nwounded_collisions,
            Nwounded_N_collisions,
            Nwounded_Nwounded_collisions,
            impact_parameter,
            event_plane_angle,
            eccentricity,
            sigma_inel_NN,
            centrality);

    att = buf;

    return true;
}

void GenHeavyIon::set( int nh, int np, int nt, int nc, int ns, int nsp,
                    int nnw, int nwn, int nwnw,
                    float im, float pl, float ec, float s, float cent ) {
    Ncoll_hard                   = nh;
    Npart_proj                   = np;
    Npart_targ                   = nt;
    Ncoll                        = nc;
    spectator_neutrons           = ns;
    spectator_protons            = nsp;
    N_Nwounded_collisions        = nnw;
    Nwounded_N_collisions        = nwn;
    Nwounded_Nwounded_collisions = nwnw;
    impact_parameter             = im;
    event_plane_angle            = pl;
    eccentricity                 = ec;
    sigma_inel_NN                = s;
    centrality                   = cent;
}

bool GenHeavyIon::operator==( const GenHeavyIon& a ) const {
  return ( memcmp( (void*) this, (void*) &a, sizeof(class GenHeavyIon) ) == 0 );
}

bool GenHeavyIon::operator!=( const GenHeavyIon& a ) const {
    return !( a == *this );
}

bool GenHeavyIon::is_valid() const {
    if( Ncoll_hard                   != 0 ) return true;
    if( Npart_proj                   != 0 ) return true;
    if( Npart_targ                   != 0 ) return true;
    if( Ncoll                        != 0 ) return true;
    if( spectator_neutrons           != 0 ) return true;
    if( spectator_protons            != 0 ) return true;
    if( N_Nwounded_collisions        != 0 ) return true;
    if( Nwounded_N_collisions        != 0 ) return true;
    if( Nwounded_Nwounded_collisions != 0 ) return true;
    if( impact_parameter             != 0 ) return true;
    if( event_plane_angle            != 0 ) return true;
    if( eccentricity                 != 0 ) return true;
    if( sigma_inel_NN                != 0 ) return true;
    if( centrality                   != 0 ) return true;
    return false;
}

} // namespace HepMC
