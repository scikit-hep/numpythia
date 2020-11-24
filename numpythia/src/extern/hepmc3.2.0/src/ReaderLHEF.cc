// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file ReaderLHEF.cc
 *  @brief Implementation of \b class ReaderLHEF
 *
 */
#include "HepMC3/ReaderLHEF.h"
using namespace LHEF;
namespace HepMC3
{
ReaderLHEF::ReaderLHEF(const std::string& filename)
{
    m_reader = new LHEF::Reader(filename);
    init();
}
ReaderLHEF::ReaderLHEF(std::istream & stream)
{
    m_reader = new LHEF::Reader(stream);
    init();
}

bool ReaderLHEF::skip(const int n)
{
    GenEvent evt;
    for (int nn=n; nn>0; --nn)
    {
        if (!read_event(evt)) return false;
        evt.clear();
    }
    return !failed();
}


void ReaderLHEF::init()
{
    m_neve=0;
    m_failed=false;
    // Create a HEPRUP attribute and initialize it from the reader.
    m_hepr = make_shared<HEPRUPAttribute>();
    m_hepr->heprup = m_reader->heprup;

    // There may be some XML tags in the LHE file which are
    // non-standard, but we can save them as well.
    m_hepr->tags = XMLTag::findXMLTags(m_reader->headerBlock + m_reader->initComments);

    // Nowwe want to create a GenRunInfo object for the HepMC file, and
    // we add the LHEF attribute to that.
    set_run_info(make_shared<GenRunInfo>());
    run_info()->add_attribute("HEPRUP", m_hepr);

    // This is just a test to make sure we can add other attributes as
    // well.
    run_info()->add_attribute("NPRUP",
                              make_shared<FloatAttribute>(m_hepr->heprup.NPRUP));

    // We want to be able to convey the different event weights to
    // HepMC. In particular we need to add the names of the weights to
    // the GenRunInfo object.
    std::vector<std::string> weightnames;
    weightnames.push_back("0"); // The first weight is always the
    // default weight with name "0".
    for ( int i = 0, N = m_hepr->heprup.weightinfo.size(); i < N; ++i )
        weightnames.push_back(m_hepr->heprup.weightNameHepMC(i));
    run_info()->set_weight_names(weightnames);

    // We also want to convey the information about which generators was
    // used.
    for ( int i = 0, N = m_hepr->heprup.generators.size(); i < N; ++i )
    {
        GenRunInfo::ToolInfo tool;
        tool.name =  m_hepr->heprup.generators[i].name;
        tool.version =  m_hepr->heprup.generators[i].version;
        tool.description =  m_hepr->heprup.generators[i].contents;
        run_info()->tools().push_back(tool);
    }
}
/// @brief Destructor
ReaderLHEF::~ReaderLHEF() {close();};

bool ReaderLHEF::read_event(GenEvent& ev)
{
    m_failed=!(m_reader->readEvent());
    if (m_failed) return m_failed;
    // To each GenEvent we want to add an attribute corresponding to
    // the HEPEUP. Also here there may be additional non-standard
    // information outside the LHEF <event> tags, which we may want to
    // add.
    shared_ptr<HEPEUPAttribute> hepe = make_shared<HEPEUPAttribute>();
    if ( m_reader->outsideBlock.length() )
        hepe->tags =  XMLTag::findXMLTags(m_reader->outsideBlock);
    hepe->hepeup = m_reader->hepeup;
    ev.set_event_number(m_neve);
    m_neve++;
    // This is just a text to check that we can add additional
    // attributes to each event.
    ev.add_attribute("HEPEUP", hepe);
    ev.add_attribute("AlphaQCD",
                     make_shared<DoubleAttribute>(hepe->hepeup.AQCDUP));
    ev.add_attribute("AlphaEM",
                     make_shared<DoubleAttribute>(hepe->hepeup.AQEDUP));
    ev.add_attribute("NUP",
                     make_shared<IntAttribute>(hepe->hepeup.NUP));
    ev.add_attribute("IDPRUP",
                     make_shared<LongAttribute>(hepe->hepeup.IDPRUP));

    // Now add the Particles from the LHE event to HepMC
    GenParticlePtr p1 = make_shared<GenParticle>(hepe->momentum(0),
                        hepe->hepeup.IDUP[0],
                        hepe->hepeup.ISTUP[0]);
    GenParticlePtr p2 = make_shared<GenParticle>(hepe->momentum(1),
                        hepe->hepeup.IDUP[1],
                        hepe->hepeup.ISTUP[1]);
    GenVertexPtr vx = make_shared<GenVertex>();
    vx->add_particle_in(p1);
    vx->add_particle_in(p2);

    for ( int i = 2; i < hepe->hepeup.NUP; ++i )
        vx->add_particle_out(make_shared<GenParticle>
                             (hepe->momentum(i),
                              hepe->hepeup.IDUP[i],
                              hepe->hepeup.ISTUP[i]));
    ev.add_vertex(vx);
    // And we also want to add the weights.
    std::vector<double> wts;
    for ( int i = 0, N = hepe->hepeup.weights.size(); i < N; ++i )
        wts.push_back(hepe->hepeup.weights[i].first);
    ev.weights() = wts;
    return m_failed;
}
/// @brief Return status of the stream
bool ReaderLHEF::failed() { return m_failed;}

/// @brief Close file stream
void ReaderLHEF::close() { delete m_reader; };
} // namespace HepMC3


