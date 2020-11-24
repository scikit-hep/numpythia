// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file WriterHEPEVT.cc
 *  @brief Implementation of \b class WriterHEPEVT
 *
 */
#include <sstream>
#include <cstdio>  // sprintf
#include "HepMC3/WriterHEPEVT.h"
#include "HepMC3/HEPEVT_Wrapper.h"
#include "HepMC3/Print.h"
namespace HepMC3
{


WriterHEPEVT::WriterHEPEVT(const std::string &filename): m_file(filename), m_stream(&m_file), m_events_count(0),m_vertices_positions_present(true)
{
    HEPMC3_WARNING( "WriterHEPEVT::WriterHEPEVT: HEPEVT format is outdated. Please use HepMC3 format instead." )
    hepevtbuffer=(char*)(new struct HEPEVT());
    HEPEVT_Wrapper::set_hepevt_address(hepevtbuffer);
}

WriterHEPEVT::WriterHEPEVT(std::ostream& stream): m_file(), m_stream(&stream), m_events_count(0),m_vertices_positions_present(true)
{
    HEPMC3_WARNING( "WriterHEPEVT::WriterHEPEVT: HEPEVT format is outdated. Please use HepMC3 format instead." )
    hepevtbuffer=(char*)(new struct HEPEVT());
    HEPEVT_Wrapper::set_hepevt_address(hepevtbuffer);
}

void WriterHEPEVT::write_hepevt_particle( int index, bool iflong )
{
    char buf[512];//Note: the format is fixed, so no reason for complicatied tratment
    char* cursor=&(buf[0]);
    cursor +=sprintf(cursor, "% 8i% 8i",HEPEVT_Wrapper::status(index), HEPEVT_Wrapper::id(index));
    if (iflong)
    {
        cursor +=sprintf(cursor,"% 8i% 8i",HEPEVT_Wrapper::first_parent(index),HEPEVT_Wrapper::last_parent(index));
        cursor +=sprintf(cursor,"% 8i% 8i",HEPEVT_Wrapper::first_child(index),HEPEVT_Wrapper::last_child(index));
        cursor +=sprintf(cursor,"% 19.8E% 19.8E% 19.8E% 19.8E% 19.8E\n",HEPEVT_Wrapper::px(index),HEPEVT_Wrapper::py(index),HEPEVT_Wrapper::pz(index),HEPEVT_Wrapper::e(index),HEPEVT_Wrapper::m(index));
        cursor +=sprintf(cursor, "%-48s% 19.8E% 19.8E% 19.8E% 19.8E\n"," ",HEPEVT_Wrapper::x(index),HEPEVT_Wrapper::y(index),HEPEVT_Wrapper::z(index),HEPEVT_Wrapper::t(index));
    }
    else
    {
        cursor +=sprintf(cursor, "% 8i% 8i",HEPEVT_Wrapper::first_child(index),HEPEVT_Wrapper::last_child(index));
        cursor +=sprintf(cursor, "% 19.8E% 19.8E% 19.8E% 19.8E\n",HEPEVT_Wrapper::px(index),HEPEVT_Wrapper::py(index),HEPEVT_Wrapper::pz(index),HEPEVT_Wrapper::m(index));
    }
    unsigned long length = cursor - &(buf[0]);
    m_stream->write( buf, length );
}

void WriterHEPEVT::write_hepevt_event_header()
{
    char buf[512];//Note: the format is fixed, so no reason for complicatied tratment
    char* cursor=buf;
    cursor +=sprintf(cursor,"E% 8i %8i\n",HEPEVT_Wrapper::event_number(),HEPEVT_Wrapper::number_entries());
    unsigned long length = cursor - &(buf[0]);
    m_stream->write( buf, length );
}

void WriterHEPEVT::write_event(const GenEvent &evt)
{
    HEPEVT_Wrapper::GenEvent_to_HEPEVT(&evt);
    HEPEVT_Wrapper::fix_daughters();
    write_hepevt_event_header();
    for( int i=1; i<=HEPEVT_Wrapper::number_entries(); ++i )  write_hepevt_particle(i,m_vertices_positions_present);
    m_events_count++;
}

void WriterHEPEVT::close()
{
    std::ofstream* ofs = dynamic_cast<std::ofstream*>(m_stream);
    if (ofs && !ofs->is_open()) return;
    if (ofs) ofs->close();
}

bool WriterHEPEVT::failed()
{
    return (bool)m_file.rdstate();
}

void WriterHEPEVT::set_vertices_positions_present(bool iflong) {m_vertices_positions_present=iflong;}

bool WriterHEPEVT::get_vertices_positions_present() const { return m_vertices_positions_present;}

} // namespace HepMC3
