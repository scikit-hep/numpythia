// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_SEARCH_FILTERBASE_H
#define  HEPMC_SEARCH_FILTERBASE_H
/**
 *  @file FilterBase.h
 *  @brief Definition of \b class FilterBase
 *
 *  @class HepMC::FilterBase
 *  @brief Base class for Filter class
 *
 *  Objects of this class cannot be directly created by user.
 *  See @ref search_engine for details on how this class is used.
 *
 *  @ingroup search_engine
 *
 */

namespace HepMC {

class Filter;

class FilterBase {
//
// Enums
//
protected:

/** @brief List of filter parameter types */
enum FilterParamType {
    INTEGER_PARAM,
    BOOL_PARAM,
    ATTRIBUTE_PARAM
};

/** @brief List of filters with integer parameter */
enum FilterIntegerParam {
    STATUS,
    PDG_ID,
    ABS_PDG_ID
};

/** @brief List of filters with boolean parameter */
enum FilterBoolParam {
    HAS_END_VERTEX,
    HAS_PRODUCTION_VERTEX,
    HAS_SAME_PDG_ID_DAUGHTER,
    IS_STABLE,
    IS_BEAM
};

/** @brief List of filters with attribute parameter */
enum FilterAttributeParam {
    ATTRIBUTE_EXISTS,
    ATTRIBUTE_IS_EQUAL
};

/** @brief List of operators on filter values */
enum FilterOperator {
    EQUAL,
    GREATER,
    GREATER_OR_EQUAL,
    LESS,
    LESS_OR_EQUAL,
    NOT_EQUAL
};

//
// Const initializers
//
public:
    static const FilterBase init_status()          { return FilterBase(STATUS);          } //!< init static const STATUS
    static const FilterBase init_pdg_id()          { return FilterBase(PDG_ID);          } //!< init static const PDG_ID
    static const FilterBase init_abs_pdg_id()      { return FilterBase(ABS_PDG_ID);      } //!< init static const ABS_PDG_ID

    static const Filter     init_has_end_vertex();           //!< init static const HAS_END_VERTEX
    static const Filter     init_has_production_vertex();    //!< init static const HAS_PRODUCTION_VERTEX
    static const Filter     init_has_same_pdg_id_daughter(); //!< init static const HAS_SAME_PDG_ID_DAUGHTER
    static const Filter     init_is_stable();                //!< init static const IS_STABLE
    static const Filter     init_is_beam();                  //!< init static const IS_BEAM

//
// Constructors
//
protected:

    /** @brief Integer-type internal constructor
     *
     *  Used by static const initializers to define filters.
     */
    FilterBase(FilterIntegerParam p):m_value_type(INTEGER_PARAM),m_int(p) {}

    /** @brief Boolean-type internal constructor
     *
     *  Used by static const initializers to define filters.
     */
    FilterBase(FilterBoolParam p):m_value_type(BOOL_PARAM),m_bool(p) {}

    /** @brief Attribute-type internal constructor
     *
     *  Used by attribute initializers to define filters.
     */
    FilterBase(FilterAttributeParam p):m_value_type(ATTRIBUTE_PARAM),m_attribute(p) {}

//
// Functions
//
public:
    Filter operator==(int value) const; //!< Create integer filter with operator ==
    Filter operator!=(int value) const; //!< Create integer filter with operator !=
    Filter operator> (int value) const; //!< Create integer filter with operator >
    Filter operator>=(int value) const; //!< Create integer filter with operator >=
    Filter operator< (int value) const; //!< Create integer filter with operator <
    Filter operator<=(int value) const; //!< Create integer filter with operator <=

// ADDED FOR NUMPYTHIA

    FilterBase(const FilterBase& filter) {
        m_value_type = filter.m_value_type;
        switch(filter.m_value_type) {
            case ATTRIBUTE_PARAM: m_attribute = filter.m_attribute;
            case INTEGER_PARAM: m_int = filter.m_int;
            case BOOL_PARAM: m_bool = filter.m_bool;
        }
    }

//
// Fields
//
protected:
    FilterParamType m_value_type; //!< Parameter type

    union {
        FilterAttributeParam m_attribute; //!< Attribute value (if attribute parameter type)
        FilterIntegerParam   m_int;       //!< Integer   value (if integer   parameter type)
        FilterBoolParam      m_bool;      //!< Boolean   value (if boolean   parameter type)
    };
};

static const FilterBase STATUS          = FilterBase::init_status();           //!< Filter base for filtering GenParticle::status()
static const FilterBase PDG_ID          = FilterBase::init_pdg_id();           //!< Filter base for filtering GenParticle::pid()
static const FilterBase ABS_PDG_ID      = FilterBase::init_abs_pdg_id();       //!< Filter base for filtering abs( GenParticle::pid() )

} // namespace HepMC

#endif
