// -*- C++ -*-
#ifndef  HEPMC_READERHEPEVT_H
#define  HEPMC_READERHEPEVT_H
/**
 *  @file  ReaderHEPEVT.h
 *  @brief Definition of \b class ReaderHEPEVT
 *
 *  @class HepMC::ReaderHEPEVT
 *  @brief GenEvent I/O parsing and serialization for HEPEVT files
 *
 *
 *  @ingroup IO
 *
 */
#include "HepMC/Reader.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenRunInfo.h"
#include "HepMC/Data/GenEventData.h"


namespace HepMC
{

class ReaderHEPEVT : public Reader
{
//
// Constructors
//
public:
    /** @brief Default constructor */
    ReaderHEPEVT(const std::string &filename);

//
// Functions
//
public:
    /** @brief Find and read event header line  from file
    *
    */
    virtual bool read_hepevt_event_header();
    /** @brief read particle from file
    *
    * @param[in] i Particle id
    * @param[in] iflong Event style
    */
    virtual bool read_hepevt_particle(int i, bool iflong=true);


    /** @brief Read event from file
     *
     *  @param[in] iflong Event style
     *  @param[out] evt Contains parsed even
     */
    bool read_event(GenEvent &evt, bool iflong);

    /** @brief Read event from file
     *
     *  @param[out] evt Contains parsed even
     */
    bool read_event(GenEvent &evt);


    /** @brief Close file stream */
    void close();

    /** @brief Get stream error state */
    bool failed();


public:
    char* hepevtbuffer; //!< Pointer to HEPEVT Fortran common block/C struct
    FILE* m_file;       //!< File to read
    bool m_failed;      //!< File state
    int m_events_count; //!< Event count
};

} // namespace HepMC

#endif
