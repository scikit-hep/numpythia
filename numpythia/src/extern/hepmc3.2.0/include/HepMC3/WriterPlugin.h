// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_WRITERPLUGIN_H
#define HEPMC3_WRITERPLUGIN_H
/**
 *  @file  WriterPlugin.h
 *  @brief Definition of \b class WriterPlugin
 *
 *  @class HepMC3::WriterPlugin
 *  @brief GenEvent I/O parsing and serialization using external plugin
 *
 *
 *  @ingroup IO
 *
 */
#include "HepMC3/Writer.h"
#include "HepMC3/GenEvent.h"
namespace HepMC3
{
class WriterPlugin : public Writer
{
public:
#ifndef HEPMC3_PYTHON_BINDINGS

/** @brief Constructor  to read from stream*/
WriterPlugin(std::ostream & stream,const std::string &libname, const std::string &newwriter, std::shared_ptr<HepMC3::GenRunInfo>run=shared_ptr<GenRunInfo>());

#endif

/** @brief Constructor to read from file*/
WriterPlugin(const std::string& filename,const std::string &libname, const std::string &newwriter, std::shared_ptr<HepMC3::GenRunInfo>run=shared_ptr<GenRunInfo>());

    /** @brief Reading event */
    void write_event(const GenEvent& ev)  override{if(!m_writer) return; return m_writer->write_event(ev);};
    /** @brief Close */
    void close() override{ if(!m_writer) return; m_writer->close();};
    /** @brief State */
    bool failed() override{if(!m_writer) return true; return m_writer->failed();};
    /** @brief Destructor */
~WriterPlugin()  override;
private:
  Writer* m_writer; ///< The actual writer
  void* dll_handle; ///< library handler
 };
}
#endif
