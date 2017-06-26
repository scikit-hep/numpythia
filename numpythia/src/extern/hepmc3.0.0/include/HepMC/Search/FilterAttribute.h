// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_SEARCH_FILTEATTRIBUTE_H
#define  HEPMC_SEARCH_FILTEATTRIBUTE_H
///
/// @file FilterAttribute.h
/// @brief Definition of \b class ATTRIBUTE
///
/// @class HepMC::ATTRIBUTE
/// @brief Filter for the attributes
///
/// Used to construct filters that can check if an attribute exists
/// or to compare against other attribute. See @ref search_engine for details
/// and examples of its use.
///
/// @ingroup search_engine
#include "HepMC/Search/Filter.h"
#include "HepMC/Attribute.h"
#include <string>

namespace HepMC {

using std::string;

class ATTRIBUTE : public Filter {
//
// Constructors
//
public:
    /// @brief Default constructor
    ///
    /// Provides the name of the attribute used in by the filter
    ATTRIBUTE(const string &name):Filter(ATTRIBUTE_EXISTS,name) {}

//
// Operators
//
public:

    /// @brief Compare if this attribute is equal to other attribute
    Filter& operator==( shared_ptr<Attribute> &at ) {
        m_attribute = ATTRIBUTE_IS_EQUAL;
        at->to_string(m_attribute_str);
        return *this;
    }

    /// @brief Compare if this attribute is not equal to other attribute
    Filter& operator!=( shared_ptr<Attribute> &at ) {
        m_bool_value = !m_bool_value;
        m_attribute  = ATTRIBUTE_IS_EQUAL;
        at->to_string(m_attribute_str);
        return *this;
    }

    /// @brief Compare if string version of this attribute is equal value
    Filter& operator==( const std::string &value ) {
        m_attribute     = ATTRIBUTE_IS_EQUAL;
        m_attribute_str = value;
        return *this;
    }

    /// @brief Compare if string version of this attribute is not equal value
    Filter& operator!=( const std::string &value ) {
        m_bool_value    = !m_bool_value;
        m_attribute     = ATTRIBUTE_IS_EQUAL;
        m_attribute_str = value;
        return *this;
    }

    /// @brief Negate logic of the result (eg. check if attribute does not exist)
    Filter& operator!() {
        m_bool_value = !m_bool_value;
        return *this;
    }
};

} // namespace HepMC

#endif
