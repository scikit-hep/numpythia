// -*- C++ -*-
#ifndef  HEPMC_WRITERHEPEVT_H
#define  HEPMC_WRITERHEPEVT_H
/**
 *  @file  WriterHEPEVT.h
 *  @brief Definition of \b class WriterHEPEVT
 *
 *  @class HepMC::WriterHEPEVT
 *  @brief GenEvent I/O serialization for HEPEVT files
 *
 *
 *  @ingroup IO
 *
 */
#include "HepMC/Writer.h"
#include "HepMC/GenEvent.h"
#include "HepMC/Data/GenEventData.h"

#include <fstream>
namespace HepMC
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
    void write_event(const GenEvent &evt);

    /** @brief Close file stream */
    void close();

    /** @brief Get stream error state flag */
    bool failed();

public:
    FILE* m_file;         //!< File to write. Need to be public to be accessible by children.

private:
    char* hepevtbuffer;   //!< Pointer to HEPEVT Fortran common block/C struct
    int   m_events_count; //!< Events count. Needed to generate unique object name
};

} // namespace HepMC
#endif
