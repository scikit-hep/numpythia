// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_SEARCH_FILTER_H
#define  HEPMC_SEARCH_FILTER_H
/**
 *  @file Filter.h
 *  @brief Definition of \b class Filter
 *
 *  @class HepMC::Filter
 *  @brief Class used to define filters for search engine
 *
 *  Objects of this class cannot be directly created by user.
 *  Use predefined static const HepMC::FilterBase objects to construct
 *  appropriate filters. See @ref search_engine for more details.
 *
 *  @ingroup search_engine
 *
 */
#include "HepMC/Search/FilterBase.h"
#include "HepMC/Data/SmartPointer.h"
#include "HepMC/Attribute.h"
#include <string>

namespace HepMC {

using std::string;

class GenEvent;

class Filter : public FilterBase {

friend class FilterBase; // To allow call to protected constructor

//
// Constructors
//
protected:
    /** @brief Internal constructor for integer-type filters
     *
     *  Invoked when an operator == != < > <= or >= with integer value
     *  is used on a filter.
     */
    Filter(FilterIntegerParam p, FilterOperator o, int value):FilterBase(p),m_operator(o),m_int_value(value),m_bool_value(true) {}

    /** @brief Internal constructor for boolean-type filters
     *
     *  Used to initialize global static const filters and to create
     *  new filters using operator '!'
     */
    Filter(FilterBoolParam p, bool value = true):FilterBase(p),m_operator(EQUAL),m_int_value(0),m_bool_value(value) {}

    /** @brief Internal constructor for attribute-type filters
     *
     *  Used when class ATTRIBUTE is called to provide attribute name
     */
    Filter(FilterAttributeParam p, const string name):FilterBase(p),m_operator(EQUAL),m_int_value(0),m_bool_value(true),m_attribute_name(name) {}

//
// Functions
//
public:
    /** @brief Check if HepMC::GenParticle passed this filter */
    bool passed_filter(const GenParticlePtr &p) const;

    /** @brief Logical NOT operator for boolean filters */
    Filter operator!() const { return Filter(m_bool,!m_bool_value); }

private:
    /** @brief Filter::passed_filter helper for integer-type filters */
    bool passed_int_filter(const GenParticlePtr &p) const;

    /** @brief Filter::passed_filter helper for pointer-type filters */
    bool passed_bool_filter(const GenParticlePtr &p) const;

    /** @brief Filter::passed_filter helper for attribute-type filters */
    bool passed_attribute_filter(const GenParticlePtr &p) const;

// ADDED FOR NUMPYTHIA
public:
    Filter(const Filter& filter):
        FilterBase(filter),
        m_operator(filter.m_operator),
        m_int_value(filter.m_int_value),
        m_bool_value(filter.m_bool_value),
        m_attribute_name(filter.m_attribute_name),
        m_attribute_str(filter.m_attribute_str){}

//
// Fields
//
protected:
    FilterOperator m_operator;       //!< Operator used by filter
    int            m_int_value;      //!< Filter parameter for integer-type filter
    bool           m_bool_value;     //!< Filter parameter for boolean-type filter
    string         m_attribute_name; //!< Filter parameter for attribute-type filters
    string         m_attribute_str;  //!< Filter parameter for attribute-type filters
};

static const Filter HAS_END_VERTEX           = FilterBase::init_has_end_vertex();           //!< Filter for checking if HepMC::GenParticle::end_vertex()        != NULL
static const Filter HAS_PRODUCTION_VERTEX    = FilterBase::init_has_production_vertex();    //!< Filter for checking if HepMC::GenParticle::production_vertex() != NULL
static const Filter HAS_SAME_PDG_ID_DAUGHTER = FilterBase::init_has_same_pdg_id_daughter(); //!< Filter for checking if end vertex has particle with same pdg_id
static const Filter IS_STABLE                = FilterBase::init_is_stable();                //!< Filter for checking if particle is stable
static const Filter IS_BEAM                  = FilterBase::init_is_beam();                  //!< Filter for checking if particle is a beam particle

} // namespace HepMC

#endif
