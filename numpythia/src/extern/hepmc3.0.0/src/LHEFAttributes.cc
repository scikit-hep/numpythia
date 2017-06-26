// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file LHEFAttributes.cc
 *  @brief Implementation of \b class HEPRUPAttribute and HEPEUPAttribute
 */

#include "HepMC/LHEFAttributes.h"
#include "HepMC/GenEvent.h"

using namespace HepMC;
using namespace LHEF;

void HEPRUPAttribute::clear() {
    for ( int i = 0, N = tags.size(); i < N; ++i ) delete tags[i];
    tags.clear();
    heprup.clear();
}

bool HEPRUPAttribute::from_string(const string &att) {
  bool found = false;
  clear();
  tags = XMLTag::findXMLTags(att);
  for ( int i = 0, N = tags.size(); i < N; ++i )
    if ( tags[i]->name == "init" ) {
      heprup = HEPRUP(*tags[i], 3);
      found = true;
    }
  return found;
}

bool HEPRUPAttribute::to_string(string &att) const {
  std::ostringstream os;
  if ( heprup.NPRUP ) heprup.print(os);
  for ( int i = 0, N = tags.size(); i < N; ++i )
    if ( heprup.NPRUP == 0 || tags[i]->name != "init" ) tags[i]->print(os);
  att = os.str();
  return true;
}

void HEPEUPAttribute::clear() {
  for ( int i = 0, N = tags.size(); i < N; ++i ) delete tags[i];
  tags.clear();
  hepeup.clear();
}

bool HEPEUPAttribute::from_string(const string &att) {
    clear();
    tags = XMLTag::findXMLTags(att);
    for ( int i = 0, N = tags.size(); i < N; ++i )
	if ( tags[i]->name == "event" || tags[i]->name == "eventgroup")
	    return true;
  return false;
}

bool HEPEUPAttribute::to_string(string &att) const {
  std::ostringstream os;
  if ( hepeup.heprup ) hepeup.print(os);
  for ( int i = 0, N = tags.size(); i < N; ++i )
    if ( !hepeup.heprup ||
	 ( tags[i]->name != "event" && tags[i]->name != "eventgroup" ) )
      tags[i]->print(os);
  att = os.str();
  return true;
}

bool HEPEUPAttribute::init(const GenEvent & geneve) {
    shared_ptr<HEPRUPAttribute> hepr =
	geneve.attribute<HEPRUPAttribute>("HEPRUP");
    bool found = false;
    for ( int i = 0, N = tags.size(); i < N; ++i )
	if ( tags[i]->name == "event" || tags[i]->name == "eventgroup" ) {
	    hepeup = HEPEUP(*tags[i], hepr->heprup);
	    found = true;
	}
    return found;
}


