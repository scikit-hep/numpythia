// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_GENPDFINFO_H
#define  HEPMC_GENPDFINFO_H
/**
 *  @file GenPdfInfo.h
 *  @brief Definition of event attribute \b class GenPdfInfo
 *
 *  @class HepMC::GenPdfInfo
 *  @brief Stores additional information about PDFs
 *
 *  This is an example of event attribute used to store PDF-related information
 *
 *  @note Input parton flavour codes id1 & id2 are expected to obey the
 *        PDG code conventions, especially g = 21.
 *
 *  @note The contents of pdf1 and pdf2 are expected to be x*f(x).
 *        The LHAPDF set ids are the entries in the first column of
 *        http:///projects.hepforge.org/lhapdf/PDFsets.index
 *
 *  @ingroup attributes
 *
 */
#include <iostream>
#include "HepMC/Attribute.h"

namespace HepMC {

class GenPdfInfo : public Attribute {

//
// Fields
//
public:
    int    parton_id[2]; ///< Parton PDG ID
    int    pdf_id[2];    ///< LHAPDF ID code
    double scale;        ///< Factorisation scale (in GEV)
    double x[2];         ///< Parton momentum fraction
    double xf[2];        ///< PDF value

//
// Functions
//
public:
    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att);

    /** @brief Implementation of Attribute::to_string */
    bool to_string(string &att) const;

    /** @brief Set all fields */
    void set( int parton_id1, int parton_id2, double x1, double x2,
              double scale_in, double xf1, double xf2,
              int pdf_id1 = 0, int pdf_id2 = 0 );

    bool operator==( const GenPdfInfo& ) const; ///< Operator ==
    bool operator!=( const GenPdfInfo& ) const; ///< Operator !=
    bool is_valid()                      const; ///< Verify that the instance contains non-zero information
};


#ifndef HEPMC_NO_DEPRECATED
typedef GenPdfInfo PdfInfo; ///< Backward compatibility typedef
#endif


} // namespace HepMC

#endif
