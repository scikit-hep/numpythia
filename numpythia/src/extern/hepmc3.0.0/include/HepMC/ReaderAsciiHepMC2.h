// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_READER_ASCII_HEPMC2_H
#define  HEPMC_READER_ASCII_HEPMC2_H
/**
 *  @file  ReaderAsciiHepMC2.h
 *  @brief Definition of \b class ReaderAsciiHepMC2
 *
 *  @class HepMC::ReaderAsciiHepMC2
 *  @brief Parser for HepMC2 I/O files
 *
 *  @ingroup IO
 *
 */
#include "HepMC/Reader.h"

#include "HepMC/Data/SmartPointer.h"

#include <string>
#include <fstream>
#include <vector>
using std::vector;
using std::ifstream;

namespace HepMC {

class GenEvent;

class ReaderAsciiHepMC2 : public Reader {
//
// Constructors
//
public:
    /** @brief Default constructor */
    ReaderAsciiHepMC2(const std::string& filename);
//
// Functions
//
public:
    /** @brief Implementation of Reader::read_event */
    bool read_event(GenEvent &evt);

    /// @brief Return status of the stream
    bool failed() { return (bool)m_file.rdstate(); }

    /// @brief Close file stream
    void close();

private:
    /** @brief Parse event
     *
     *  Helper routine for parsing event information
     *  @param[out] evt Event that will be filled with new data
     *  @param[in]  buf Line of text that needs to be parsed
     */
    int parse_event_information(GenEvent &evt, const char *buf);

    /** @brief Parse units
     *
     *  Helper routine for parsing unit information
     *  @param[out] evt Event that will be filled with unit information
     *  @param[in]  buf Line of text that needs to be parsed
     */
    bool parse_units(GenEvent &evt, const char *buf);

    /** @brief Parse vertex
     *
     *  Helper routine for parsing single event information
     *  @param[in] buf Line of text that needs to be parsed
     */
    int parse_vertex_information(const char *buf);

    /** @brief Parse particle
     *
     *  Helper routine for parsing single particle information
     *  @param[in] buf Line of text that needs to be parsed
     */
    int parse_particle_information(const char *buf);

    /** @brief Parse weight names
     *
     *  Helper routine for parsing weight names
     *  @param[in] buf Line of text that needs to be parsed
     */
    bool parse_weight_names(const char *buf);

    /** @brief Parse heavy ion information
     *
     *  Helper routine for parsing heavy ion information
     *  @param[out] evt Event that will be filled with new data
     *  @param[in]  buf Line of text that needs to be parsed
     */
    bool parse_heavy_ion(GenEvent &evt, const char *buf);

    /** @brief Parse pdf information
     *
     *  Helper routine for parsing pdf information
     *  @param[out] evt Event that will be filled with new data
     *  @param[in]  buf Line of text that needs to be parsed
     */
    bool parse_pdf_info(GenEvent &evt, const char *buf);


    /** @brief Parse pdf information
    *
    *  Helper routine for parsing cross-section information
    *  @param[out] evt Event that will be filled with new data
    *  @param[in]  buf Line of text that needs to be parsed
    */
    bool parse_xs_info(GenEvent &evt, const char *buf);



//
// Fields
//
private:
    ifstream               m_file;                //!< Input file

    vector<GenVertexPtr>   m_vertex_cache;        //!< Vertex cache
    vector<int>            m_vertex_barcodes;     //!< Old vertex barcodes

    vector<GenParticlePtr> m_particle_cache;      //!< Particle cache
    vector<int>            m_end_vertex_barcodes; //!< Old end vertex barcodes
};

} // namespace HepMC

#endif
