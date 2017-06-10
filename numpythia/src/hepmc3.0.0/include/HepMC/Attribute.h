// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_ATTRIBUTE_H
#define  HEPMC_ATTRIBUTE_H
/**
 *  @file Attribute.h
 *  @brief Definition of \b class Attribute, \b class IntAttribute and \b class StringAttribute
 *
 *  @class HepMC::Attribute
 *  @brief Base class for all attributes
 *
 *  Contains virtual functions to_string and from_string that
 *  each attribute must implement, as well as init function that
 *  attributes should overload to initialize parsed attribute
 *
 *  @ingroup attributes
 *
 */
#include <cstdio> // sprintf
#include <string>
#include <limits>
#include <sstream>
#include <iomanip>
#include "HepMC/Common.h"
using std::string;

namespace HepMC {

/** @brief Forward declaration of GenEvent. */
class GenEvent;

/** @brief Forward declaration of GenRunInfo. */
class GenRunInfo;

class Attribute {
//
// Constructors
//
public:
    /** @brief Default constructor */
    Attribute():m_is_parsed(true) {}

    /** @brief Virtual destructor */
    virtual ~Attribute() {}

protected:
    /** @brief Protected constructor that allows to set string
     *
     *  Used when parsing attributes from file. An StringAttribute class
     *  object is made, which uses this constructor to signify that
     *  it just holds string without parsing it.
     *
     *  @note There should be no need for user class to ever use this constructor
     */
    Attribute(const string &st):m_is_parsed(false),m_string(st) {}

//
// Virtual Functions
//
public:
    /** @brief Fill class content from string.
     */
    virtual bool from_string(const string & att) = 0;

    /** @brief Optionally initialize the attribute after from_string
     *
     * Is passed a reference to the GenEvent object to which the
     * Attribute belongs.
     */
  virtual bool init(const GenEvent & /*geneve*/) {
	return true;
    }

    /** @brief Optionally initialize the attribute after from_string
     *
     * Is passed a reference to the GenRunInfo object to which the
     * Attribute belongs.
     */
  virtual bool init(const GenRunInfo & /*genrun*/) {
	return true;
    }

    /** @brief Fill string from class content */
    virtual bool to_string(string &att) const = 0;

//
// Accessors
//
public:
    /** @brief Check if this attribute is parsed */
    bool is_parsed() { return m_is_parsed; }

    /** @brief Get unparsed string */
    const string& unparsed_string() const { return m_string; }

protected:
    /** @brief Set is_parsed flag */
    void set_is_parsed(bool flag) { m_is_parsed = flag; }

    /** @brief Set unparsed string */
    void set_unparsed_string(const string &st) { m_string = st; }

//
// Fields
//
private:
    bool   m_is_parsed; //!< Is this attribute parsed?
    string m_string;    //!< Raw (unparsed) string
};

/**
 *  @class HepMC::IntAttribute
 *  @brief Attribute that holds an Integer implemented as an int
 *
 *  @ingroup attributes
 */
class IntAttribute : public Attribute {
public:

    /** @brief Default constructor */
    IntAttribute():Attribute(),m_val(0) {}

    /** @brief Constructor initializing attribute value */
    IntAttribute(int val):Attribute(),m_val(val) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
#ifdef HEPMC_HAS_CXX11
        m_val = std::stoi(att);
#else
        m_val = atoi( att.c_str() );
#endif
        return true;
    }

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const {
#ifdef HEPMC_HAS_CXX11
        att = std::to_string(m_val);
#else
        char buf[24];
        sprintf(buf,"%23i",m_val);
        att = buf;
#endif
        return true;
    }

    /** @brief get the value associated to this Attribute. */
    int value() const {
	return m_val;
    }

    /** @brief set the value associated to this Attribute. */
    void set_value(int i) {
	m_val = i;
    }

private:
    int m_val; ///< Attribute value
};

/**
 *  @class HepMC::IntAttribute
 *  @brief Attribute that holds an Integer implemented as an int
 *
 *  @ingroup attributes
 */
class LongAttribute : public Attribute {
public:

    /** @brief Default constructor */
    LongAttribute(): Attribute(), m_val(0) {}

    /** @brief Constructor initializing attribute value */
    LongAttribute(long val): Attribute(), m_val(val) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
#ifdef HEPMC_HAS_CXX11
        m_val = std::stoi(att);
#else
        m_val = atoi( att.c_str() );
#endif
        return true;
    }

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const {
#ifdef HEPMC_HAS_CXX11
        att = std::to_string(m_val);
#else
        char buf[24];
        sprintf(buf,"%23li",m_val);
        att = buf;
#endif
        return true;
    }

    /** @brief get the value associated to this Attribute. */
    long value() const {
	return m_val;
    }

    /** @brief set the value associated to this Attribute. */
    void set_value(long l) {
	m_val = l;
    }

private:

    long m_val; ///< Attribute value

};

/**
 *  @class HepMC::DoubleAttribute
 *  @brief Attribute that holds a real number as a double.
 *
 *  @ingroup attributes
 */
class DoubleAttribute : public Attribute {
public:

    /** @brief Default constructor */
    DoubleAttribute(): Attribute(), m_val(0.0) {}

    /** @brief Constructor initializing attribute value */
    DoubleAttribute(double val): Attribute(), m_val(val) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
#ifdef HEPMC_HAS_CXX11
        m_val = std::stod(att);
#else
        m_val = atof( att.c_str() );
#endif
        return true;
    }

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const {
      std::ostringstream oss;
      oss << std::setprecision(std::numeric_limits<double>::digits10)
	  << m_val;
      att = oss.str();
      return true;
    }

    /** @brief get the value associated to this Attribute. */
    double value() const {
	return m_val;
    }

    /** @brief set the value associated to this Attribute. */
    void set_value(double d) {
	m_val = d;
    }

private:

    double m_val; ///< Attribute value
};

/**
 *  @class HepMC::FloatAttribute
 *  @brief Attribute that holds a real number as a float.
 *
 *  @ingroup attributes
 */
class FloatAttribute : public Attribute {
public:

    /** @brief Default constructor */
    FloatAttribute(): Attribute(), m_val(0.0) {}

    /** @brief Constructor initializing attribute value */
    FloatAttribute(float val): Attribute(), m_val(val) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
#ifdef HEPMC_HAS_CXX11
        m_val = std::stof(att);
#else
        m_val = float(atof( att.c_str() ));
#endif
        return true;
    }

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const {
      std::ostringstream oss;
      oss << std::setprecision(std::numeric_limits<float>::digits10)
	  << m_val;
      att = oss.str();
      return true;
    }

    /** @brief get the value associated to this Attribute. */
    float value() const {
	return m_val;
    }

    /** @brief set the value associated to this Attribute. */
    void set_value(float f) {
	m_val = f;
    }

private:

    float m_val; ///< Attribute value
};

/**
 *  @class HepMC::StringAttribute
 *  @brief Attribute that holds a string
 *
 *  Default attribute constructed when reading input files.
 *  It can be then parsed by other attributes or left as a string.
 *
 *  @ingroup attributes
 *
 */
class StringAttribute : public Attribute {
public:

    /** @brief Default constructor - empty string */
    StringAttribute():Attribute() {}

    /** @brief String-based constructor
     *
     *  The Attribute constructor used here marks that this is an unparsed
     *  string that can be (but does not have to be) parsed
     *
     */
    StringAttribute(const string &st):Attribute(st) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
        set_unparsed_string(att);
        return true;
    }

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const {
        att = unparsed_string();
        return true;
    }

    /** @brief get the value associated to this Attribute. */
    string value() const {
	return unparsed_string();
    }

    /** @brief set the value associated to this Attribute. */
    void set_value(string s) {
	set_unparsed_string(s);
    }

};

} // namespace HepMC

#endif
