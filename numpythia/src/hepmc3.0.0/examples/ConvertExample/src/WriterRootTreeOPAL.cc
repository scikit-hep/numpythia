// -*- C++ -*-
//
#include "WriterRootTreeOPAL.h"
namespace HepMC
{
WriterRootTreeOPAL::WriterRootTreeOPAL(const std::string &filename,shared_ptr<GenRunInfo> run):WriterRootTree::WriterRootTree(filename,"h10","h10",run) {}
void WriterRootTreeOPAL::init_branches()
{
    m_tree->Branch("Irun", &m_Irun);
    m_tree->Branch("Ievnt", &m_Ievnt);
    m_tree->Branch("Ebeam",&m_Ebeam);
}
void WriterRootTreeOPAL::write_event(const GenEvent &evt)
{
    m_Ievnt=evt.event_number();
    if (evt.particles().size()>0) m_Ebeam=std::abs(evt.particles().at(0)->momentum().e());
    WriterRootTree::write_event(evt);
}
void WriterRootTreeOPAL::set_run_number(const int nr) {m_Irun=nr;}
} // namespace HepMC
