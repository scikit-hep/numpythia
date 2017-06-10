// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_SEARCH_FINDPARTICLES_H
#define  HEPMC_SEARCH_FINDPARTICLES_H
/**
 *  @file FindParticles.h
 *  @brief Definition of \b class FindParticles
 *
 *  @class HepMC::FindParticles
 *  @brief Search engine for GenEvent class
 *
 *  @ingroup search_engine
 *
 */
#include "HepMC/Search/FilterList.h"
#include "HepMC/Data/SmartPointer.h"

namespace HepMC {


class GenEvent;


/** @brief List of methods of searching through all particles in the event */
enum FilterType {
    FIND_ALL,
    FIND_FIRST,
    FIND_LAST
};
/// Compatibility name
using FilterEvent = FilterType;


class FindParticles {
//
// Constructors
//
public:

    /** @brief GenEvent-based constructor */
    FindParticles(const GenEvent &evt, FilterEvent filter_type, FilterList filter_list = FilterList() );

    /** @brief GenParticle-based constructor */
    FindParticles(const GenParticlePtr &p, Relationship filter_type, FilterList filter_list = FilterList() );

    /** @brief GenVertex-based constructor */
    FindParticles(const GenVertexPtr &v, Relationship filter_type, FilterList filter_list = FilterList() );


    /** @brief Narrow down the results applying additional filters */
    void narrow_down( FilterList filter_list );
//
// Functions
//
private:
    /** @brief Check if particle passed all filters */
    bool passed_all_filters(const GenParticlePtr &p, FilterList &filter_list);

    /** @brief Check if all ancestors passed the filter
     *
     *  Recursively check all particles and production vertices of these particles
     */
    void recursive_check_ancestors(const GenVertexPtr &v, FilterList &filter_list);

    /** @brief Check if all descendants passed the filter
     *
     *  Recursively check all particles and end vertices of these particles
     */
    void recursive_check_descendants(const GenVertexPtr &v, FilterList &filter_list);
//
// Accessors
//
public:
    const vector<GenParticlePtr>& results() const { return m_results; } //!< Get results

//
// Fields
//
private:
    vector<GenParticlePtr> m_results;          //!< List of results
    vector<GenVertexPtr>   m_checked_vertices; //!< List of already checked vertices
};


  /// @name Finding via unbound functions (returns by copy)
  //@{

  /** @brief Find from GenEvent */
  inline vector<GenParticlePtr> findParticles(const GenEvent &evt, FilterEvent filter_type, FilterList filter_list = FilterList() ) {
    return FindParticles(evt, filter_type, filter_list).results();
  }

  /** @brief Find from GenParticle */
  inline vector<GenParticlePtr> findParticles(const GenParticlePtr &p, Relationship filter_type, FilterList filter_list = FilterList() ) {
    return FindParticles(p, filter_type, filter_list).results();
  }

  /** @brief Find from GenVertex */
  inline vector<GenParticlePtr> findParticles(const GenVertexPtr &v, Relationship filter_type, FilterList filter_list = FilterList() ) {
    return FindParticles(v, filter_type, filter_list).results();
  }

  //@}


} // namespace HepMC

#endif
