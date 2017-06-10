#include "WriterHEPEVTZEUS.h"
#include "HepMC/HEPEVT_Wrapper.h"
namespace HepMC
{
WriterHEPEVTZEUS::WriterHEPEVTZEUS(const std::string &filename):WriterHEPEVT(filename) {}
void WriterHEPEVTZEUS::write_hepevt_event_header()
{
    fprintf(m_file," E % 12i% 12i% 12i\n",HEPEVT_Wrapper::event_number(),0,HEPEVT_Wrapper::number_entries());
}
  void WriterHEPEVTZEUS::write_hepevt_particle( int index, bool /*iflong*/ )
{
    fprintf(m_file,"% 12i% 8i",HEPEVT_Wrapper::status(index), HEPEVT_Wrapper::id(index));
    fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::first_parent(index),HEPEVT_Wrapper::last_parent(index));
    fprintf(m_file,"% 8i% 8i",HEPEVT_Wrapper::first_child(index),HEPEVT_Wrapper::last_child(index));
    fprintf(m_file,      "% 19.11E% 19.11E% 19.11E% 19.11E% 19.11E\n",HEPEVT_Wrapper::px(index),HEPEVT_Wrapper::py(index),HEPEVT_Wrapper::pz(index),HEPEVT_Wrapper::e(index),HEPEVT_Wrapper::m(index));
    fprintf(m_file, "%-52s% 19.11E% 19.11E% 19.11E% 19.11E% 19.11E\n"," ",HEPEVT_Wrapper::x(index),HEPEVT_Wrapper::y(index),HEPEVT_Wrapper::z(index),HEPEVT_Wrapper::t(index),0.0);
}
}// namespace HepMC
