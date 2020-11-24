// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_PRINTSTREAMS_H
#define HEPMC3_PRINTSTREAMS_H
///
/// @file PrintStreams.h
/// @brief Implementation of ostreams for the objects
///

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/Print.h"

namespace HepMC3 {
/// @brief Print content of all GenEvent containers with idiomatic C++ printing.
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, const GenEvent &event) { Print::content(os,event); return os; }

/// @brief Print one-line info with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os,  ConstGenVertexPtr v) { Print::line(os,v); return os; }

/// @brief Print one-line info with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os,  ConstGenParticlePtr p) { Print::line(os,p); return os; }

/// @brief Print one-line info with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, shared_ptr<GenCrossSection> &cs) { Print::line(os,cs); return os; }

/// @brief Print one-line info with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, shared_ptr<GenHeavyIon> &hi) { Print::line(os,hi); return os; }

/// @brief Print one-line info  with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, shared_ptr<GenPdfInfo> &pi) { Print::line(os,pi); return os; }

/// @brief Print one-line info  with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, const FourVector& p) { Print::line(os,p); return os; }

/// @brief Print one-line info  with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os, const GenRunInfo::ToolInfo& t) { Print::line(os,t); return os; }

/// @brief Print one-line info  with idiomatic C++ printing
/// @note More generic printing methods from HepMC3::Print should be preffered
inline std::ostream& operator<<(std::ostream& os,const GenRunInfo &ri) { Print::line(os,ri); return os; }



} // namespace HepMC3

#endif
