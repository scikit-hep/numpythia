// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file FilterBase.cc
 *  @brief Definition of \b class FilterBase
 *
 */
#include "HepMC/Search/FilterBase.h"
#include "HepMC/Search/Filter.h"

namespace HepMC {

// Define operators for integer-type filters
Filter FilterBase::operator==(int value) const { return Filter(m_int,EQUAL,           value); }
Filter FilterBase::operator!=(int value) const { return Filter(m_int,NOT_EQUAL,       value); }
Filter FilterBase::operator> (int value) const { return Filter(m_int,GREATER,         value); }
Filter FilterBase::operator>=(int value) const { return Filter(m_int,GREATER_OR_EQUAL,value); }
Filter FilterBase::operator< (int value) const { return Filter(m_int,LESS,            value); }
Filter FilterBase::operator<=(int value) const { return Filter(m_int,LESS_OR_EQUAL,   value); }

// Define initializators for boolean-type filters
const Filter FilterBase::init_has_end_vertex()           { return Filter(HAS_END_VERTEX);           }
const Filter FilterBase::init_has_production_vertex()    { return Filter(HAS_PRODUCTION_VERTEX);    }
const Filter FilterBase::init_has_same_pdg_id_daughter() { return Filter(HAS_SAME_PDG_ID_DAUGHTER); }
const Filter FilterBase::init_is_stable()                { return Filter(IS_STABLE);                }
const Filter FilterBase::init_is_beam()                  { return Filter(IS_BEAM);                  }

} // namespace HepMC
