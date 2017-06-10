// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_PRINT_H
#define  HEPMC_PRINT_H
///
/// @file Print.h
/// @brief Definition of static \b class Print
///

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

namespace HepMC {


class GenPdfInfo;
class GenHeavyIon;
class GenCrossSection;


/// @brief Provides different printing formats
/// @todo This class has no state -- why isn't it just a namespace with free functions?
class Print {
public:
    /// @brief Print content of all GenEvent containers
    static void content(const GenEvent &event);

    /// @brief Print event in listing (HepMC2) format
    static void listing(const GenEvent &event, unsigned short precision = 2);

    /// @brief Print one-line info
    static void line(const GenEvent &event);

    /// @brief Print one-line info
    static void line(const GenVertexPtr &v);

    /// @brief Print one-line info
    static void line(const GenParticlePtr &p);

    /// @brief Print one-line info
    static void line(shared_ptr<GenCrossSection> &cs);

    /// @brief Print one-line info
    static void line(shared_ptr<GenHeavyIon> &hi);

    /// @brief Print one-line info
    static void line(shared_ptr<GenPdfInfo> &pi);

private:
    /// @brief Helper function for printing a vertex in listing format
    static void listing(const GenVertexPtr &v);

    /// @brief Helper function for printing a particle in listing format
    static void listing(const GenParticlePtr &p);

    virtual ~Print() {}
};


} // namespace HepMC

#endif
