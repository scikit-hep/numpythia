// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_PRINT_H
#define HEPMC3_PRINT_H
///
/// @file Print.h
/// @brief Definition of static \b class Print
///

#include "HepMC3/GenEvent.h"
#include "HepMC3/GenRunInfo.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenParticle.h"

namespace HepMC3 {


class GenPdfInfo;
class GenHeavyIon;
class GenCrossSection;


/// @brief Provides different printing formats
class Print {
public:
    /// @brief Print content of all GenEvent containers
    static void content(std::ostream& os, const GenEvent &event);
    /// @brief Print content of all GenEvent containers to std::cout
    inline static void content(const GenEvent &event) { content(std::cout, event); }

    /// @brief Print event in listing (HepMC2) format
    static void listing(std::ostream& os, const GenEvent &event, unsigned short precision = 2);
    /// @brief Print event in listing (HepMC2) format  to std::cout
    inline static void listing(const GenEvent &event, unsigned short precision = 2) {
        listing(std::cout, event, precision);
    }

    /// @brief Print runinfo in listing
    static void listing(std::ostream& os, const GenRunInfo &ri, unsigned short precision = 2);
    /// @brief Print runinfo in listing  to std::cout
    inline static void listing(const GenRunInfo &ri, unsigned short precision = 2) {
        listing(std::cout, ri, precision);
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, const GenEvent &event, bool attributes=false);
    /// @brief Print one-line info to std::cout
    inline static void line(const GenEvent &event, bool attributes=false) {
        line(std::cout, event, attributes);
        std::cout<<std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, const GenRunInfo &RunInfo, bool attributes=false);
    /// @brief Print one-line info to std::cout
    inline static void line(const GenRunInfo &RunInfo, bool attributes=false) {
        line(std::cout, RunInfo, attributes);
        std::cout<<std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, const GenRunInfo::ToolInfo& t);
    /// @brief Print one-line info to std::cout
    inline static void line(const GenRunInfo::ToolInfo& t) {
        line(std::cout, t);
        std::cout << std::endl;
    }



    /// @brief Print one-line info
    static void line(std::ostream& os, ConstGenVertexPtr v, bool attributes=false);
    /// @brief Print one-line info to std::cout
    inline static void line(ConstGenVertexPtr v, bool attributes=false) {
        line(std::cout, v, attributes);
        std::cout << std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, const FourVector& p);
    /// @brief Print one-line info to std::cout
    inline static void line(const FourVector& p) {
        line(std::cout, p);
        std::cout << std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, ConstGenParticlePtr p, bool attributes=false);
    /// @brief Print one-line info to std::cout
    inline static void line(ConstGenParticlePtr p, bool attributes=false) {
        line(std::cout, p, attributes);
        std::cout << std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, shared_ptr<GenCrossSection> &cs);
    /// @brief Print one-line info to std::cout
    inline static void line(shared_ptr<GenCrossSection> &cs) {
        line(std::cout, cs);
        std::cout<<std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, shared_ptr<GenHeavyIon> &hi);
    /// @brief Print one-line info to std::cout
    inline static void line(shared_ptr<GenHeavyIon> &hi) {
        line(std::cout, hi);
        std::cout<<std::endl;
    }

    /// @brief Print one-line info
    static void line(std::ostream& os, shared_ptr<GenPdfInfo> &pi);
    /// @brief Print one-line info to std::cout
    inline static void line(shared_ptr<GenPdfInfo> &pi) {
        line(std::cout, pi);
        std::cout<<std::endl;
    }
///deleted copy assignment operator
    Print& operator = (const Print &) = delete;            

private:
    /// @brief Helper function for printing a vertex in listing format
    static void listing(std::ostream& os, ConstGenVertexPtr v);

    /// @brief Helper function for printing a particle in listing format
    static void listing(std::ostream& os, ConstGenParticlePtr p);

    /// @brief Destructor
    virtual ~Print() {}
};
} // namespace HepMC3

#endif
