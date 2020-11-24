// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
///
/// @file WriterAsciiHepMC2.cc
/// @brief Implementation of \b class WriterAsciiHepMC2
///
#include "HepMC3/WriterAsciiHepMC2.h"

#include "HepMC3/Version.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Units.h"
#include <cstring>

namespace HepMC3
{


WriterAsciiHepMC2::WriterAsciiHepMC2(const std::string &filename, shared_ptr<GenRunInfo> run)
    : m_file(filename),
      m_stream(&m_file),
      m_precision(16),
      m_buffer(nullptr),
      m_cursor(nullptr),
      m_buffer_size( 256*1024 ),
      m_particle_counter(0)
{
    HEPMC3_WARNING( "WriterAsciiHepMC2::WriterAsciiHepMC2: HepMC2 format is outdated. Please use HepMC3 format instead." )
    set_run_info(run);
    if ( !run_info() ) set_run_info(make_shared<GenRunInfo>());
    if ( !m_file.is_open() )
    {
        HEPMC3_ERROR( "WriterAsciiHepMC2: could not open output file: "<<filename )
    }
    else
    {
        m_file << "HepMC::Version " << version() << std::endl;
        m_file << "HepMC::IO_GenEvent-START_EVENT_LISTING" << std::endl;
    }
}

WriterAsciiHepMC2::WriterAsciiHepMC2(std::ostream &stream, shared_ptr<GenRunInfo> run)
    : m_file(),
      m_stream(&stream),
      m_precision(16),
      m_buffer(nullptr),
      m_cursor(nullptr),
      m_buffer_size( 256*1024 ),
      m_particle_counter(0)
{
    HEPMC3_WARNING( "WriterAsciiHepMC2::WriterAsciiHepMC2: HepMC2 format is outdated. Please use HepMC3 format instead." )
    set_run_info(run);
    if ( !run_info() ) set_run_info(make_shared<GenRunInfo>());
    (*m_stream) << "HepMC::Version " << version() << std::endl;
    (*m_stream) << "HepMC::IO_GenEvent-START_EVENT_LISTING" << std::endl;
}


WriterAsciiHepMC2::~WriterAsciiHepMC2()
{
    close();
    if ( m_buffer ) delete[] m_buffer;
}


void WriterAsciiHepMC2::write_event(const GenEvent &evt)
{
    allocate_buffer();
    if ( !m_buffer ) return;

    // Make sure nothing was left from previous event
    flush();

    if ( !run_info() ) set_run_info(evt.run_info());
    if ( evt.run_info() && run_info() != evt.run_info() ) set_run_info(evt.run_info());


    shared_ptr<DoubleAttribute> A_event_scale=evt.attribute<DoubleAttribute>("event_scale");
    shared_ptr<DoubleAttribute> A_alphaQED=evt.attribute<DoubleAttribute>("alphaQED");
    shared_ptr<DoubleAttribute> A_alphaQCD=evt.attribute<DoubleAttribute>("alphaQCD");
    shared_ptr<IntAttribute> A_signal_process_id=evt.attribute<IntAttribute>("signal_process_id");
    shared_ptr<IntAttribute> A_mpi=evt.attribute<IntAttribute>("mpi");
    shared_ptr<IntAttribute> A_signal_process_vertex=evt.attribute<IntAttribute>("signal_process_vertex");

    double event_scale=A_event_scale?(A_event_scale->value()):0.0;
    double alphaQED=A_alphaQED?(A_alphaQED->value()):0.0;
    double alphaQCD=A_alphaQCD?(A_alphaQCD->value()):0.0;
    int signal_process_id=A_signal_process_id?(A_signal_process_id->value()):0;
    int mpi=A_mpi?(A_mpi->value()):0;
    int signal_process_vertex=A_signal_process_vertex?(A_signal_process_vertex->value()):0;

    std::vector<long> m_random_states;
    for (int i=0; i<100; i++)
    {
        shared_ptr<IntAttribute> rs=evt.attribute<IntAttribute>("random_states"+to_string((long long unsigned int)i));
        if (!rs) break;
        m_random_states.push_back(rs->value());
    }
    // Write event info
    //Find beam particles
    std::vector<int> beams;
    int idbeam=0;
    for(ConstGenVertexPtr v: evt.vertices() )
    {
        for(ConstGenParticlePtr p: v->particles_in())
        {
            if (!p->production_vertex())         { if (p->status()==4) beams.push_back(idbeam); idbeam++;}
            else if (p->production_vertex()->id()==0) { if (p->status()==4) beams.push_back(idbeam); idbeam++;}
        }
        for( ConstGenParticlePtr p: v->particles_out()) { if (p->status()==4) beams.push_back(idbeam); idbeam++;}
    }
    //
    int idbeam1=10000;
    int idbeam2=10000;
    if (beams.size()>0) idbeam1+=beams[0]+1;
    if (beams.size()>1) idbeam2+=beams[1]+1;
    m_cursor += sprintf(m_cursor, "E %d %d %e %e %e %d %d %lu %i %i",
                        evt.event_number(),
                        mpi,
                        event_scale,
                        alphaQCD,
                        alphaQED,
                        signal_process_id,
                        signal_process_vertex,
                        evt.vertices().size(),
                        idbeam1,idbeam2
                       );

    flush();
    m_cursor += sprintf(m_cursor, " %zu",m_random_states.size());
    for (size_t  q=0; q<m_random_states.size(); q++)
    {
        m_cursor += sprintf(m_cursor, " %ii",(int)q);
    }
    flush();
    if ( evt.weights().size() )
    {
        m_cursor += sprintf(m_cursor, " %lu",evt.weights().size());
        for (double w: evt.weights())
            m_cursor += sprintf(m_cursor, " %.*e",m_precision, w);
        m_cursor += sprintf(m_cursor, "\n");
        flush();
    }
    m_cursor += sprintf(m_cursor, "N %lu",evt.weights().size());
    vector<string> names = run_info()->weight_names();
    for (size_t q=0; q<evt.weights().size(); q++)
    {
        if (q<names.size())
            m_cursor += sprintf(m_cursor, " \"%s\"",names[q].c_str());
        else
            m_cursor += sprintf(m_cursor, " \"%i\"",(int)q);
    }
    m_cursor += sprintf(m_cursor, "\n");
    flush();
    // Write units
    m_cursor += sprintf(m_cursor, "U %s %s\n", Units::name(evt.momentum_unit()).c_str(), Units::name(evt.length_unit()).c_str());
    flush();
    shared_ptr<GenCrossSection> cs = evt.attribute<GenCrossSection>("GenCrossSection");
    if(cs) {m_cursor += sprintf(m_cursor, "C %.*e %.*e\n",m_precision, cs->xsec(),m_precision,cs->xsec_err());  flush(); }


    // Write attributes
    for ( auto vt1: evt.attributes() )
    {
        for ( auto vt2: vt1.second )
        {

            string st;
            bool status = vt2.second->to_string(st);

            if( !status )
            {
                HEPMC3_WARNING( "WriterAsciiHepMC2::write_event: problem serializing attribute: "<<vt1.first )
            }
            else
            {
                if (vt1.first=="GenPdfInfo")
                {
                    m_cursor +=
                        sprintf(m_cursor, "F ");
                    flush();
                    write_string(escape(st));
                    m_cursor += sprintf(m_cursor, "\n");
                    flush();
                }
            }
        }
    }
    m_particle_counter=0;
    for(ConstGenVertexPtr v: evt.vertices() )
    {
        int production_vertex = 0;
        production_vertex=v->id();
        write_vertex(v);
        for(ConstGenParticlePtr p: v->particles_in())
        {
            if (!p->production_vertex()) write_particle( p, production_vertex );
            else
            {
                if (p->production_vertex()->id()==0)write_particle( p, production_vertex );
            }
        }
        for(ConstGenParticlePtr p: v->particles_out())
            write_particle( p, production_vertex );
    }

    // Flush rest of the buffer to file
    forced_flush();
}


void WriterAsciiHepMC2::allocate_buffer()
{
    if ( m_buffer ) return;
    while( m_buffer==nullptr && m_buffer_size >= 256 ) {
        try {
            m_buffer = new char[ m_buffer_size ]();
        }     catch (const std::bad_alloc& e) {
            delete[] m_buffer;
            m_buffer_size /= 2;
            HEPMC3_WARNING( "WriterAsciiHepMC2::allocate_buffer:"<<e.what()<<" buffer size too large. Dividing by 2. New size: " << m_buffer_size )
        }
    }

    if ( !m_buffer )
    {
        HEPMC3_ERROR( "WriterAsciiHepMC2::allocate_buffer: could not allocate buffer!" )
        return;
    }

    m_cursor = m_buffer;
}


string WriterAsciiHepMC2::escape(const string& s) const
{
    string ret;
    ret.reserve( s.length()*2 );
    for ( string::const_iterator it = s.begin(); it != s.end(); ++it )
    {
        switch ( *it )
        {
        case '\\':
            ret += "\\\\";
            break;
        case '\n':
            ret += "\\|";
            break;
        default:
            ret += *it;
        }
    }
    return ret;
}

void WriterAsciiHepMC2::write_vertex(ConstGenVertexPtr v)
{
    std::vector<double> weights;
    for (int i=0; i<100; i++)
    {
        shared_ptr<DoubleAttribute> rs=v->attribute<DoubleAttribute>("weight"+to_string((long long unsigned int)i));
        if (!rs) break;
        weights.push_back(rs->value());
    }

    m_cursor += sprintf( m_cursor, "V %i %i",v->id(),v->status() );
    flush();
    int orph=0;
    for(ConstGenParticlePtr p: v->particles_in())
    {
        if (!p->production_vertex()) orph++;
        else
        {
            if (p->production_vertex()->id()==0)orph++;
        }
    }
    const FourVector &pos = v->position();
    if (pos.is_zero())
    {
        m_cursor += sprintf(m_cursor," 0 0 0 0");
    }
    else
    {
        m_cursor += sprintf(m_cursor," %.*e",m_precision,pos.x());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",   m_precision,pos.y());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",   m_precision,pos.z());
        flush();
        m_cursor += sprintf(m_cursor," %.*e", m_precision,pos.t());
        flush();
    }
    m_cursor += sprintf(m_cursor," %i %lu %lu",orph,v->particles_out().size(),weights.size());
    flush();
    for (size_t i=0; i<weights.size(); i++) m_cursor += sprintf(m_cursor," %.*e",   m_precision,weights[i]);
    m_cursor += sprintf(m_cursor,"\n");
    flush();
}


inline void WriterAsciiHepMC2::flush()
{
    // The maximum size of single add to the buffer (other than by
    // using WriterAsciiHepMC2::write) is 32 bytes. This is a safe value as
    // we will not allow precision larger than 24 anyway
    unsigned long length = m_cursor - m_buffer;
    if ( m_buffer_size - length < 32 )
    {
        m_stream->write( m_buffer, length );
        m_cursor = m_buffer;
    }
}


inline void WriterAsciiHepMC2::forced_flush()
{
    // m_file.write( m_buffer, m_cursor-m_buffer );
    m_stream->write( m_buffer, m_cursor - m_buffer );
    m_cursor = m_buffer;
}


void WriterAsciiHepMC2::write_run_info() {}

void WriterAsciiHepMC2::write_particle(ConstGenParticlePtr p, int second_field)
{

    m_cursor += sprintf(m_cursor,"P %i",int(10001+m_particle_counter));
    m_particle_counter++;
    flush();
    m_cursor += sprintf(m_cursor," %i",   p->pid() );
    flush();
    m_cursor += sprintf(m_cursor," %.*e", m_precision,p->momentum().px() );
    flush();
    m_cursor += sprintf(m_cursor," %.*e", m_precision,p->momentum().py());
    flush();
    m_cursor += sprintf(m_cursor," %.*e", m_precision,p->momentum().pz() );
    flush();
    m_cursor += sprintf(m_cursor," %.*e", m_precision,p->momentum().e() );
    flush();
    m_cursor += sprintf(m_cursor," %.*e", m_precision,p->generated_mass() );
    flush();
    m_cursor += sprintf(m_cursor," %i", p->status() );
    flush();
    int ev=0;
    if (p->end_vertex())
        if (p->end_vertex()->id()!=0)
            ev=p->end_vertex()->id();

    shared_ptr<DoubleAttribute> A_theta=p->attribute<DoubleAttribute>("theta");
    shared_ptr<DoubleAttribute> A_phi=p->attribute<DoubleAttribute>("phi");
    if (A_theta) m_cursor += sprintf(m_cursor," %.*e", m_precision, A_theta->value());
    else m_cursor += sprintf(m_cursor," 0");
    flush();
    if (A_phi) m_cursor += sprintf(m_cursor," %.*e", m_precision, A_phi->value());
    else m_cursor += sprintf(m_cursor," 0");
    flush();
    m_cursor += sprintf(m_cursor," %i", ev );
    flush();

    shared_ptr<IntAttribute> A_flow1=p->attribute<IntAttribute>("flow1");
    shared_ptr<IntAttribute> A_flow2=p->attribute<IntAttribute>("flow2");
    int flowsize=0;
    if (A_flow1) flowsize++;
    if (A_flow2) flowsize++;
    m_cursor += sprintf(m_cursor," %i", flowsize);
    if (A_flow1) m_cursor += sprintf(m_cursor," 1 %i", A_flow1->value());
    if (A_flow2) m_cursor += sprintf(m_cursor," 2 %i", A_flow2->value());
    m_cursor += sprintf(m_cursor,"\n");
    flush();
}


inline void WriterAsciiHepMC2::write_string( const string &str )
{

    // First let's check if string will fit into the buffer
    unsigned long length = m_cursor-m_buffer;

    if ( m_buffer_size - length > str.length() )
    {
        strncpy(m_cursor,str.data(),str.length());
        m_cursor += str.length();
        flush();
    }
    // If not, flush the buffer and write the string directly
    else
    {
        forced_flush();
        m_stream->write( str.data(), str.length() );
    }
}


void WriterAsciiHepMC2::close()
{
    std::ofstream* ofs = dynamic_cast<std::ofstream*>(m_stream);
    if (ofs && !ofs->is_open()) return;
    forced_flush();
    (*m_stream) << "HepMC::IO_GenEvent-END_EVENT_LISTING" << endl << endl;
    if (ofs) ofs->close();
}
bool WriterAsciiHepMC2::failed() { return (bool)m_file.rdstate(); }

void WriterAsciiHepMC2::set_precision(const int& prec ) {
    if (prec < 2 || prec > 24) return;
    m_precision = prec;
}

int WriterAsciiHepMC2::precision() const {
    return m_precision;
}

void WriterAsciiHepMC2::set_buffer_size(const size_t& size ) {
    if (m_buffer) return;
    if (size < 256) return;
    m_buffer_size = size;
}
} // namespace HepMC3
