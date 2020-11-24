// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
///
/// @file WriterAscii.cc
/// @brief Implementation of \b class WriterAscii
///
#include "HepMC3/WriterAscii.h"

#include "HepMC3/Version.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/Units.h"
#include <cstring>
#include <algorithm>//min max for VS2017
namespace HepMC3 {


WriterAscii::WriterAscii(const std::string &filename, shared_ptr<GenRunInfo> run)
    : m_file(filename),
      m_stream(&m_file),
      m_precision(16),
      m_buffer(nullptr),
      m_cursor(nullptr),
      m_buffer_size( 256*1024 )
{
    set_run_info(run);
    if ( !m_file.is_open() ) {
        HEPMC3_ERROR( "WriterAscii: could not open output file: "<<filename )
    } else {
        m_file << "HepMC::Version " << version() << std::endl;
        m_file << "HepMC::Asciiv3-START_EVENT_LISTING" << std::endl;
        if ( run_info() ) write_run_info();
    }
}


WriterAscii::WriterAscii(std::ostream &stream, shared_ptr<GenRunInfo> run)
    : m_file(),
      m_stream(&stream),
      m_precision(16),
      m_buffer(nullptr),
      m_cursor(nullptr),
      m_buffer_size( 256*1024 )

{
    set_run_info(run);
    (*m_stream) << "HepMC::Version " << version() << std::endl;
    (*m_stream) << "HepMC::Asciiv3-START_EVENT_LISTING" << std::endl;
    if ( run_info() ) write_run_info();
}


WriterAscii::~WriterAscii() {
    close();
    if ( m_buffer ) delete[] m_buffer;
}


void WriterAscii::write_event(const GenEvent &evt) {

    // if ( !m_file.is_open() ) return;
    allocate_buffer();
    if ( !m_buffer ) return;

    // Make sure nothing was left from previous event
    flush();

    if ( !run_info() ) {
        set_run_info(evt.run_info());
        write_run_info();
    } else {
        if ( evt.run_info() && (run_info() != evt.run_info()) ) {
            HEPMC3_WARNING( "WriterAscii::write_event: GenEvents contain "
                            "different GenRunInfo objects from - only the "
                            "first such object will be serialized." )
        }
        // else {
        //write_run_info();
        //    }
    }

    // Write event info
    m_cursor += sprintf(m_cursor, "E %d %lu %lu", evt.event_number(), evt.vertices().size(), evt.particles().size());
    flush();

    // Write event position if not zero
    const FourVector &pos = evt.event_pos();
    if ( !pos.is_zero() ) {
        m_cursor += sprintf(m_cursor," @ %.*e",m_precision,pos.x());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",  m_precision,pos.y());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",  m_precision,pos.z());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",  m_precision,pos.t());
        flush();
    }

    m_cursor += sprintf(m_cursor,"\n");
    flush();

    // Write units
    m_cursor += sprintf(m_cursor, "U %s %s\n", Units::name(evt.momentum_unit()).c_str(), Units::name(evt.length_unit()).c_str());
    flush();

    // Write weight values if present
    if ( evt.weights().size() ) {
        m_cursor += sprintf(m_cursor, "W");
        for (auto  w: evt.weights())
            m_cursor += sprintf(m_cursor, " %.*e",std::min(3*m_precision,22), w);
        m_cursor += sprintf(m_cursor, "\n");
        flush();
    }

    // Write attributes
    for ( auto vt1: evt.attributes() ) {
        for ( auto vt2: vt1.second ) {

            string st;
            bool status = vt2.second->to_string(st);

            if( !status ) {
                HEPMC3_WARNING( "WriterAscii::write_event: problem serializing attribute: "<<vt1.first )
            }
            else {
                m_cursor +=
                    sprintf(m_cursor, "A %i %s ",vt2.first,vt1.first.c_str());
                flush();
                write_string(escape(st));
                m_cursor += sprintf(m_cursor, "\n");
                flush();
            }
        }
    }


    // Print particles
    std::map<ConstGenVertexPtr,bool>  alreadywritten;
    for(ConstGenParticlePtr p: evt.particles() ) {

        // Check to see if we need to write a vertex first
        ConstGenVertexPtr v = p->production_vertex();
        int parent_object = 0;

        if (v) {
            // Check if we need this vertex at all
            //Yes, use vertex as parent object
            if ( v->particles_in().size() > 1 || !v->data().is_zero() ) parent_object = v->id();
            //No, use particle as parent object
            //TODO: add check for attributes of this vertex
            else if ( v->particles_in().size() == 1 )                   parent_object = v->particles_in()[0]->id();
            //Usage of map instead of simple countewr helps to deal with events with random ids of vertices.
            if (alreadywritten.find(v)==alreadywritten.end()&&parent_object<0)
            { write_vertex(v); alreadywritten[v]=true;}
        }

        write_particle( p, parent_object );
    }
    alreadywritten.clear();

    // Flush rest of the buffer to file
    forced_flush();
}


void WriterAscii::allocate_buffer() {
    if ( m_buffer ) return;
    while( m_buffer==nullptr && m_buffer_size >= 256 ) {
        try {
            m_buffer = new char[ m_buffer_size ]();
        }     catch (const std::bad_alloc& e) {
            delete[] m_buffer;
            m_buffer_size /= 2;
            HEPMC3_WARNING( "WriterAscii::allocate_buffer:"<<e.what()<<" buffer size too large. Dividing by 2. New size: " << m_buffer_size )
        }
    }

    if ( !m_buffer ) {
        HEPMC3_ERROR( "WriterAscii::allocate_buffer: could not allocate buffer!" )
        return;
    }
    m_cursor = m_buffer;
}


string WriterAscii::escape(const string& s)  const {
    string ret;
    ret.reserve( s.length()*2 );
    for ( string::const_iterator it = s.begin(); it != s.end(); ++it ) {
        switch ( *it ) {
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

void WriterAscii::write_vertex(ConstGenVertexPtr v) {

    m_cursor += sprintf( m_cursor, "V %i %i [",v->id(),v->status() );
    flush();

    bool printed_first = false;
    std::vector<int> pids;
    for(ConstGenParticlePtr p: v->particles_in() ) pids.push_back(p->id());
//We order pids to be able to compare ascii files
    std::sort(pids.begin(),pids.end());
    for(auto pid: pids ) {

        if ( !printed_first ) {
            m_cursor  += sprintf(m_cursor,"%i", pid);
            printed_first = true;
        }
        else m_cursor += sprintf(m_cursor,",%i",pid);

        flush();
    }

    const FourVector &pos = v->position();
    if ( !pos.is_zero() ) {
        m_cursor += sprintf(m_cursor,"] @ %.*e",m_precision,pos.x());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",   m_precision,pos.y());
        flush();
        m_cursor += sprintf(m_cursor," %.*e",   m_precision,pos.z());
        flush();
        m_cursor += sprintf(m_cursor," %.*e\n", m_precision,pos.t());
        flush();
    }
    else {
        m_cursor += sprintf(m_cursor,"]\n");
        flush();
    }
}


inline void WriterAscii::flush() {
    // The maximum size of single add to the buffer (other than by
    // using WriterAscii::write) is 32 bytes. This is a safe value as
    // we will not allow precision larger than 24 anyway
    unsigned long length = m_cursor - m_buffer;
    if ( m_buffer_size - length < 32 ) {
        // m_file.write( m_buffer, length );
        m_stream->write( m_buffer, length );
        m_cursor = m_buffer;
    }
}


inline void WriterAscii::forced_flush() {
    // m_file.write( m_buffer, m_cursor-m_buffer );
    m_stream->write( m_buffer, m_cursor - m_buffer );
    m_cursor = m_buffer;
}


void WriterAscii::write_run_info() {

    allocate_buffer();

    // If no run info object set, create a dummy one.
    if ( !run_info() ) set_run_info(make_shared<GenRunInfo>());

    vector<string> names = run_info()->weight_names();

    if ( !names.empty() ) {
        string out = names[0];
        for ( int i = 1, N = names.size(); i < N; ++i )
            out += "\n" + names[i];
        m_cursor += sprintf(m_cursor, "W ");
        flush();
        write_string(escape(out));
        m_cursor += sprintf(m_cursor, "\n");
    }

    for ( int i = 0, N = run_info()->tools().size(); i < N; ++i  ) {
        string out = "T " + run_info()->tools()[i].name + "\n"
                     + run_info()->tools()[i].version + "\n"
                     + run_info()->tools()[i].description;
        write_string(escape(out));
        m_cursor += sprintf(m_cursor, "\n");
    }


    for ( auto att: run_info()->attributes() ) {
        string st;
        if ( ! att.second->to_string(st) ) {
            HEPMC3_WARNING ("WriterAscii::write_run_info: problem serializing attribute: "<< att.first )
        }
        else {
            m_cursor +=
                sprintf(m_cursor, "A %s ", att.first.c_str());
            flush();
            write_string(escape(st));
            m_cursor += sprintf(m_cursor, "\n");
            flush();
        }
    }
}

void WriterAscii::write_particle(ConstGenParticlePtr p, int second_field) {

    m_cursor += sprintf(m_cursor,"P %i",p->id());
    flush();

    m_cursor += sprintf(m_cursor," %i",   second_field);
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
    m_cursor += sprintf(m_cursor," %i\n", p->status() );
    flush();
}


inline void WriterAscii::write_string( const string &str ) {

    // First let's check if string will fit into the buffer
    unsigned long length = m_cursor-m_buffer;

    if ( m_buffer_size - length > str.length() ) {
        strncpy(m_cursor,str.data(),str.length());
        m_cursor += str.length();
        flush();
    }
    // If not, flush the buffer and write the string directly
    else {
        forced_flush();
        // m_file.write( str.data(), str.length() );
        m_stream->write( str.data(), str.length() );
    }
}


void WriterAscii::close() {
    std::ofstream* ofs = dynamic_cast<std::ofstream*>(m_stream);
    if (ofs && !ofs->is_open()) return;
    forced_flush();
    (*m_stream) << "HepMC::Asciiv3-END_EVENT_LISTING" << endl << endl;
    if (ofs) ofs->close();
}
bool WriterAscii::failed() { return (bool)m_file.rdstate(); }

void WriterAscii::set_precision(const int& prec ) {
    if (prec < 2 || prec > 24) return;
    m_precision = prec;
}

int WriterAscii::precision() const {
    return m_precision;
}

void WriterAscii::set_buffer_size(const size_t& size ) {
    if (m_buffer) return;
    if (size < 256) return;
    m_buffer_size = size;
}


} // namespace HepMC3
