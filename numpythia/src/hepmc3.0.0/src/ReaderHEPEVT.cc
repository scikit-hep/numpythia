// -*- C++ -*-
/**
 *  @file ReaderHEPEVT.cc
 *  @brief Implementation of \b class ReaderHEPEVT
 *
 */
#include "HepMC/ReaderHEPEVT.h"
#include "HepMC/HEPEVT_Wrapper.h"

#include <sstream>
namespace HepMC
{

ReaderHEPEVT::ReaderHEPEVT(const std::string &filename):
    m_events_count(0)
{
    m_failed=false;
    set_run_info(make_shared<GenRunInfo>());

    m_file= fopen(filename.c_str(),"r");
    if (!m_file) {m_failed=true;  ERROR( "ReaderHEPEVT: file opening failed" ); }
    else
        {
            hepevtbuffer=(char*)(new struct HEPEVT());
            HEPEVT_Wrapper::set_hepevt_address(hepevtbuffer);
        }

}

#define  READERHEPEVTBUFFERSIZE 255
bool ReaderHEPEVT::read_hepevt_event_header()
{
    char buf_e[READERHEPEVTBUFFERSIZE];
    bool eventline=false;
    int m_i=0, m_p=0;
    while(!eventline)
        {
            if (fgets(buf_e,READERHEPEVTBUFFERSIZE,m_file)==NULL) break;
            std::stringstream st_e(buf_e);
            char attr=' ';
            eventline=false;
            while (!eventline)
                {
                    if (!(st_e>>attr)) break;
                    if (attr==' ') continue;
                    else eventline=false;
                    if (attr=='E')
                        {
                            eventline=static_cast<bool>(st_e>>m_i>>m_p);
                        }
                }
        }
    HEPEVT_Wrapper::set_event_number(m_i);
    HEPEVT_Wrapper::set_number_entries(m_p);
    return eventline;
}


bool ReaderHEPEVT::read_hepevt_particle( int i, bool iflong )
{
    char buf_p[READERHEPEVTBUFFERSIZE];
    char buf_v[READERHEPEVTBUFFERSIZE];
    int   intcodes[6];
    double fltcodes1[5];
    double fltcodes2[4];
    if (fgets(buf_p,READERHEPEVTBUFFERSIZE,m_file)==NULL) return false;
    if (iflong) if (fgets(buf_v,READERHEPEVTBUFFERSIZE,m_file)==NULL) return false;
    std::stringstream st_p(buf_p);
    std::stringstream st_v(buf_v);
    if (iflong)
        {
            if (!static_cast<bool>(st_p>>intcodes[0]>>intcodes[1]>>intcodes[2]>>intcodes[3]>>intcodes[4]>>intcodes[5]>>fltcodes1[0]>>fltcodes1[1]>>fltcodes1[2]>>fltcodes1[3]>>fltcodes1[4])) { ERROR( "ReaderHEPEVT: Error reading particle momenta"); 	return false;}
            if (!static_cast<bool>(st_v>>fltcodes2[0]>>fltcodes2[1]>>fltcodes2[2]>>fltcodes2[3])) { ERROR( "ReaderHEPEVT: Error reading particle vertex");  return false;}
        }
    else
        {
            if (!static_cast<bool>(st_p>>intcodes[0]>>intcodes[1]>>intcodes[4]>>intcodes[5]>>fltcodes1[0]>>fltcodes1[1]>>fltcodes1[2]>>fltcodes1[4])) {ERROR( "ReaderHEPEVT: Error reading particle momenta"); 	return false;}
            intcodes[2]=0;//FIXME!
            intcodes[3]=0;//FIXME!
            fltcodes1[3]=std::sqrt(fltcodes1[0]*fltcodes1[0]+fltcodes1[1]*fltcodes1[1]+fltcodes1[2]*fltcodes1[2]+fltcodes1[4]*fltcodes1[4]);
            fltcodes2[0]=0;
            fltcodes2[1]=0;
            fltcodes2[2]=0;
            fltcodes2[3]=0;
        }
    HEPEVT_Wrapper::set_status(i,intcodes[0]);
    HEPEVT_Wrapper::set_id(i,intcodes[1]);
    HEPEVT_Wrapper::set_parents(i,intcodes[2],std::max(intcodes[2],intcodes[3]));/* Pythia writes second mother 0*/
    HEPEVT_Wrapper::set_children(i,intcodes[4],intcodes[5]);
    HEPEVT_Wrapper::set_momentum(i,fltcodes1[0],fltcodes1[1],fltcodes1[2],fltcodes1[3]);
    HEPEVT_Wrapper::set_mass(i,fltcodes1[4]);
    HEPEVT_Wrapper::set_position(i,fltcodes2[0],fltcodes2[1],fltcodes2[2],fltcodes2[3]);
    return true;

}

bool ReaderHEPEVT::read_event(GenEvent& evt, bool iflong)
{
    evt.clear();


    bool fileok=true;
    HEPEVT_Wrapper::zero_everything();
    fileok=read_hepevt_event_header();
    for (int i=1; (i<=HEPEVT_Wrapper::number_entries())&&fileok; i++)
        fileok=read_hepevt_particle(i, iflong);
    bool result=false;
    if (fileok)
        {
            m_events_count++;
            result=HEPEVT_Wrapper::HEPEVT_to_GenEvent(&evt);
            shared_ptr<GenRunInfo> g=make_shared<GenRunInfo>();
            std::vector<std::string> weightnames;
            weightnames.push_back("0");
            std::vector<double> wts;
            wts.push_back(1.0);
            g->set_weight_names(weightnames);
            evt.set_run_info(g);
            evt.weights()=wts;
        }
    else m_failed=true;
    return result;
}
bool ReaderHEPEVT::read_event(GenEvent& evt)
{return read_event(evt,true); }


void ReaderHEPEVT::close()
{
    if (m_file) fclose(m_file);
}

bool ReaderHEPEVT::failed()
{
    return m_failed;
}

} // namespace HepMC
