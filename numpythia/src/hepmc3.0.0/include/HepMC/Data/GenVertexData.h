// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_DATA_GENVERTEXDATA_H
#define  HEPMC_DATA_GENVERTEXDATA_H
/**
 *  @file GenVertexData.h
 *  @brief Definition of \b class GenVertexData
 *
 *  @struct HepMC::GenVertexData
 *  @brief Stores serializable vertex information
 *
 *  @ingroup data
 *
 */
#include "HepMC/FourVector.h"

namespace HepMC {

struct GenVertexData {
    int        status;   ///< Vertex status
    FourVector position; ///< Position in time-space
    
    /// @brief Check if this struct fields are zero
    bool is_zero() const {
        if( status ) return false;
        
        return position.is_zero();
    }
};

} // namespace HepMC

#endif
