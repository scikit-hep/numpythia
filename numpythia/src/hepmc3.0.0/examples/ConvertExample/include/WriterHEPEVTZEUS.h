#ifndef  HEPMC_WRITERHEPEVTZEUS_H
#define  HEPMC_WRITERHEPEVTZEUS_H
#include "HepMC/WriterHEPEVT.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/Data/GenEventData.h"
namespace HepMC
{
class WriterHEPEVTZEUS : public  WriterHEPEVT
{
public:
    WriterHEPEVTZEUS(const std::string &filename);
    void write_hepevt_event_header();
    void write_hepevt_particle( int index, bool iflong );
};
}
#endif
