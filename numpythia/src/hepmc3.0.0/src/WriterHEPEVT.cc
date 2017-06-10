// -*- C++ -*-
//
/**
 *  @file WriterHEPEVT.cc
 *  @brief Implementation of \b class WriterHEPEVT
 *
 */
#include "HepMC/WriterHEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"
#include <cstdio>  // sprintf
#include "HepMC/Print.h"
#include <sstream>
namespace HepMC
{

WriterHEPEVT::WriterHEPEVT(const std::string &filename): m_events_count(0)
{
    m_file=fopen(filename.c_str(),"w");
    hepevtbuffer=(char*)(new struct HEPEVT());
    HEPEVT_Wrapper::set_hepevt_address(hepevtbuffer);
}

void WriterHEPEVT::write_hepevt_particle( int index, bool iflong )
{
    fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::status(index), HEPEVT_Wrapper::id(index));
    if (iflong)
        {
            fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::first_parent(index),HEPEVT_Wrapper::last_parent(index));
            fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::first_child(index),HEPEVT_Wrapper::last_child(index));
            fprintf(m_file,"% 19.8E% 19.8E% 19.8E% 19.8E% 19.8E\n",HEPEVT_Wrapper::px(index),HEPEVT_Wrapper::py(index),HEPEVT_Wrapper::pz(index),HEPEVT_Wrapper::e(index),HEPEVT_Wrapper::m(index));
            fprintf(m_file, "%-48s% 19.8E% 19.8E% 19.8E% 19.8E\n"," ",HEPEVT_Wrapper::x(index),HEPEVT_Wrapper::y(index),HEPEVT_Wrapper::z(index),HEPEVT_Wrapper::t(index));
        }
    else
        {
            fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::first_child(index),HEPEVT_Wrapper::last_child(index));
            fprintf(m_file,"% 19.8E% 19.8E% 19.8E% 19.8E\n",HEPEVT_Wrapper::px(index),HEPEVT_Wrapper::py(index),HEPEVT_Wrapper::pz(index),HEPEVT_Wrapper::m(index));
        }
}

void WriterHEPEVT::write_hepevt_event_header()
{
    fprintf(m_file,"E% 8i %8i\n",HEPEVT_Wrapper::event_number(),HEPEVT_Wrapper::number_entries());
}

void WriterHEPEVT::write_event(const GenEvent &evt)
{
    HEPEVT_Wrapper::GenEvent_to_HEPEVT(&evt);
    HEPEVT_Wrapper::fix_daughters();
    write_hepevt_event_header();
    for( int i=1; i<=HEPEVT_Wrapper::number_entries(); ++i )  write_hepevt_particle(i);
    m_events_count++;
}

void WriterHEPEVT::close()
{
    fclose(m_file);
}

bool WriterHEPEVT::failed()
{
    return false;
}

} // namespace HepMC
