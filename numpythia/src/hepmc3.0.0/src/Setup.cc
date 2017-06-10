// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file Setup.cc
 *  @brief Implementation of Setup class
 *
 */
#include "HepMC/Setup.h"

namespace HepMC {

const unsigned int Setup::DEFAULT_DOUBLE_ALMOST_EQUAL_MAXULPS = 10;
const double       Setup::DOUBLE_EPSILON                      = 10e-20;

bool Setup::m_is_printing_errors    = true;
bool Setup::m_is_printing_warnings  = true;
int  Setup::m_debug_level           = 5;

} // namespace HepMC
