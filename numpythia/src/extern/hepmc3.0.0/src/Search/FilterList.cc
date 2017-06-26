// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file FilterList.cc
 *  @brief Implementation of \b class FilterList
 *
 */
#include "HepMC/Search/FilterList.h"
#include "HepMC/Search/Filter.h"

namespace HepMC {

FilterList::FilterList(const Filter &f1) {
    m_filters.push_back(f1);
}

FilterList::FilterList(const Filter &f1, const Filter &f2) {
    m_filters.push_back(f1); m_filters.push_back(f2);
}

FilterList& FilterList::operator&&(const Filter &f) {
    m_filters.push_back(f);
    return *this;
}

FilterList operator&&(const Filter &f1, const Filter &f2) {
    return FilterList(f1,f2);
}

} // namespace HepMC
