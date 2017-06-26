// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_LHEFATTRIBUTES_H
#define  HEPMC_LHEFATTRIBUTES_H
/**
 *  @file LHEFAttributes.h
 *  @brief Definition of \b class HEPRUPAttribute and \b class HEPEUAttribute
 *
 *  @ingroup attributes
 *
 */

#include "HepMC/Attribute.h"
#include "LHEF.h"
#include "FourVector.h"

namespace HepMC {

/**
 *  @class HepMC::HEPRUPAttribute
 *  @brief Class for storing data for LHEF run information
 */
class HEPRUPAttribute: public Attribute {

public:

  /** @brief Default constructor */
  HEPRUPAttribute() {}

  /** @brief Constructor from string*/
  HEPRUPAttribute(std::string s): Attribute(s) {}

  /** @brief Virtual destructor */
  virtual ~HEPRUPAttribute() {
    clear();
  }

  //
  // Virtual Functions
  //
public:
  /** @brief Fill class content from string */
  virtual bool from_string(const string &att);
  
  /** @brief Fill string from class content */
  virtual bool to_string(string &att) const;

public:

  /** @brief Clear this object. */
  void clear();

  /** @brief The actual HEPRUP object. */
  LHEF::HEPRUP heprup;

  /** @brief The parsed XML-tags. */
  std::vector<LHEF::XMLTag*> tags;

};

/**
 *  @class HepMC::HEPEUPAttribute
 *  @brief Class for storing data for LHEF run information
 */
class HEPEUPAttribute: public Attribute {

public:

  /** @brief Default constructor */
  HEPEUPAttribute() {}

  /** @brief Constructor from string*/
  HEPEUPAttribute(std::string s): Attribute(s) {}

  /** @brief Virtual destructor */
  virtual ~HEPEUPAttribute() {
    clear();
  }

  //
  // Virtual Functions
  //
public:
  /** @brief Fill class content from string */
  virtual bool from_string(const string &att);
  
  /** @brief Parse the XML-tags. */
  virtual bool init(const GenEvent & geneve);

  /** @brief Dummy function. */
  virtual bool init(const GenRunInfo & /*runinfo*/) {
    return true;
  }

  /** @brief Fill string from class content */
  virtual bool to_string(string &att) const;

public:

  /** @brief Get momentum */
  FourVector momentum(int i) const {
    return FourVector(hepeup.PUP[i][0], hepeup.PUP[i][1],
		      hepeup.PUP[i][2], hepeup.PUP[i][3]);
  }

  /** @brief Clear this object. */
  void clear();

  /** @brief The actual HEPEUP object. */
  LHEF::HEPEUP hepeup;

  /** @brief The parsed XML-tags. */
  std::vector<LHEF::XMLTag*> tags;

      
};

} // namespace HepMC

#endif




