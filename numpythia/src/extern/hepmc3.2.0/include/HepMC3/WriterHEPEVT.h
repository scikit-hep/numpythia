// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_WRITERHEPEVT_H
#define HEPMC3_WRITERHEPEVT_H
/**
 *  @file  WriterHEPEVT.h
 *  @brief Definition of \b class WriterHEPEVT
 *
 *  @class HepMC3::WriterHEPEVT
 *  @brief GenEvent I/O serialization for HEPEVT files
 *
 *
 *  @ingroup IO
 *
 */
#include "HepMC3/Writer.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/Data/GenEventData.h"

#include <fstream>
namespace HepMC3
{

class WriterHEPEVT : public Writer
{
//
// Constructors
//
public:
    /** @brief Default constructor
     *  @warning If file exists, it will be overwritten
     */
    WriterHEPEVT(const std::string &filename);

#ifndef HEPMC3_PYTHON_BINDINGS
    /// @brief Constructor from ostream
    WriterHEPEVT(std::ostream& stream);
#endif
//
// Functions
//
public:

    /** @brief Write particle to file
     *
     *  @param[in] index Particle to be serialized
     *  @param[in] iflong Format of record
     */

    virtual void write_hepevt_particle( int index, bool iflong=true );
    /** @brief Write event header to file
     *
     */
    virtual void write_hepevt_event_header();

    /** @brief Write event to file
     *
     *  @param[in] evt Event to be serialized
     */
    void write_event(const GenEvent &evt)  override;

    /** @brief Close file stream */
    void close()  override;

    /** @brief Get stream error state flag */
    bool failed()  override;
    /** @brief  set flag if vertex positions are available */
    void set_vertices_positions_present(bool iflong);

    /** @brief  get flag if vertex positions are available */
    bool get_vertices_positions_present() const;

protected:
    std::ofstream m_file; //!< Output file
    std::ostream* m_stream; //!< Output stream
    char* hepevtbuffer;   //!< Pointer to HEPEVT Fortran common block/C struct
    int   m_events_count; //!< Events count. Needed to generate unique object name
    bool m_vertices_positions_present; //!< true if vertex positions are available
};

} // namespace HepMC3
#endif
