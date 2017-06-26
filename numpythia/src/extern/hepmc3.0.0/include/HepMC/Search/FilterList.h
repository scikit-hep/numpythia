// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_SEARCH_FILTERLIST_H
#define  HEPMC_SEARCH_FILTERLIST_H
/**
 *  @file FilterList.h
 *  @brief Definition of \b class FilterList
 *
 *  @class HepMC::FilterList
 *  @brief List of filters for the search engine
 *
 *  Constructs lists out of Filter objects
 *
 *  @ingroup search_engine
 *
 */
#include "HepMC/Search/Filter.h"
#include "HepMC/Search/FilterAttribute.h"

#include <vector>
using std::vector;

namespace HepMC {

class FilterList {
//
// Constructors
//
public:
    /** @brief Default constructor
     *
     *  Used when no filters were passed to search enginge
     */
    FilterList() {}

    /** @brief Single filter constructor
     *
     *  Used as casting operator when just one filter
     *  was passed to search engine
     */
    FilterList(const Filter &f1);

    /** @brief Two filters constructor
     *
     *  Used by global Filter AND operator to quickly join pair of filters
     */
    FilterList(const Filter &f1, const Filter &f2);

    /** @brief AND operator
     *
     *  Used to join multiple filters
     */
    FilterList& operator&&(const Filter &f);

// ADDED FOR NUMPYTHIA

    FilterList& extend(FilterList& flist) {
        const vector<Filter> filters = flist.filters();
        vector<Filter>::const_iterator it(filters.begin());
        for(; it != filters.end(); ++it) {
            m_filters.push_back(*it);
        }
        return *this;
    }

    FilterList& append(const Filter& f) {
        m_filters.push_back(f);
        return *this;
    }

//
// Accessors
//
public:
    const vector<Filter>& filters() { return m_filters; } //!< Get list of filters

//
// Fields
//
private:
    vector<Filter> m_filters; //!< List of filters
};

/** @brief Filter AND operator
 *
 *  Defined so that FilterList can be constructed
 *  when there is more than one filter passed to search engine
 */
FilterList operator&&(const Filter &f1, const Filter &f2);

} // namespace HepMC

#endif
