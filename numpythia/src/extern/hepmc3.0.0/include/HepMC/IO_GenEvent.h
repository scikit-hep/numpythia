// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_IO_GENEVENT_H
#define  HEPMC_IO_GENEVENT_H
///
/// @file  IO_GenEvent.h
/// @brief Definition of backward-compatibility class \b IO_GenEvent
///
/// @class HepMC::IO_GenEvent
/// @brief Backward-compatibility wrapper for WriterAscii and ReaderAscii
///
/// @ingroup IO
///
#include "HepMC/WriterAscii.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/Common.h"

namespace HepMC {


    class GenEvent;


    class IO_GenEvent {
    public:

        /// @brief Filename constructor allowing both ios::in and ios::out modes
        // HEPMC_DEPRECATED("Use ReaderAscii or WriterAscii instead")
        IO_GenEvent(const string &filename, std::ios::openmode mode = std::ios::out)
          : m_writer(NULL), m_reader(NULL)
        {
            if ( mode == std::ios::out )     m_writer = new WriterAscii(filename);
            else if ( mode == std::ios::in ) m_reader = new ReaderAscii(filename);
        }

        /// @brief Istream constructor
        // HEPMC_DEPRECATED("Use ReaderAscii instead")
        IO_GenEvent(std::istream &stream)
          : m_writer(NULL), m_reader(NULL)
        {
            m_reader = new ReaderAscii(stream);
        }

        /// @brief Ostream constructor
        // HEPMC_DEPRECATED("Use WriterAscii instead")
        IO_GenEvent(std::ostream &stream)
          : m_writer(NULL), m_reader(NULL)
        {
            m_writer = new WriterAscii(stream);
        }

        /// @brief Default destructor
        ~IO_GenEvent() {
            if(m_writer) delete m_writer;
            if(m_reader) delete m_reader;
        }


        /// @brief Wrapper for ReaderAscii::read_event
        bool fill_next_event( GenEvent *evt ) {
            if( !m_reader ) return false;
            return m_reader->read_event(*evt);
        }

        /// @brief Wrapper for WriterAscii::write_event
        void write_event( const GenEvent *evt ) {
            if( !m_writer ) return;
            m_writer->write_event(*evt);
        }

        /// @brief Wrapper for Writer/Reader rdstate()
        int rdstate() const {
            if( m_writer ) return m_writer->failed();
            if( m_reader ) return m_reader->failed();
            return 1;
        }


    private:

      /// @brief Instance of WriterAscii
      /// @todo Use unique_ptr
      WriterAscii *m_writer;

      /// @brief Instance of ReaderAscii
      /// @todo Use unique_ptr
      ReaderAscii *m_reader;

    };


} // namespace HepMC

#endif
