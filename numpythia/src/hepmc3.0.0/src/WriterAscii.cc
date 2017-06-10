// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
///
/// @file WriterAscii.cc
/// @brief Implementation of \b class WriterAscii
///
#include "HepMC/WriterAscii.h"

#include "HepMC/Version.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Units.h"
#include <cstring>

namespace HepMC {


WriterAscii::WriterAscii(const std::string &filename, shared_ptr<GenRunInfo> run)
  : m_file(filename),
    m_stream(&m_file),
    m_precision(16),
    m_buffer(NULL),
    m_cursor(NULL),
    m_buffer_size( 256*1024 )
{
    set_run_info(run);
    if ( !m_file.is_open() ) {
        ERROR( "WriterAscii: could not open output file: "<<filename )
    } else {
        m_file << "HepMC::Version " << HepMC::version() << std::endl;
        m_file << "HepMC::IO_GenEvent-START_EVENT_LISTING" << std::endl;
	    if ( run_info() ) write_run_info();
    }
}


WriterAscii::WriterAscii(std::ostream &stream, shared_ptr<GenRunInfo> run)
  : m_file(),
    m_stream(&stream),
    m_precision(16),
    m_buffer(NULL),
    m_cursor(NULL),
    m_buffer_size( 256*1024 )
{
    set_run_info(run);
    // if ( !m_file.is_open() ) {
    //     ERROR( "WriterAscii: could not open output file: "<<filename )
	// } else {
        // m_file << "HepMC::Version " << HepMC::version() << std::endl;
        // m_file << "HepMC::IO_GenEvent-START_EVENT_LISTING" << std::endl;
    (*m_stream) << "HepMC::Version " << HepMC::version() << std::endl;
    (*m_stream) << "HepMC::IO_GenEvent-START_EVENT_LISTING" << std::endl;
	if ( run_info() ) write_run_info();
    // }
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
	if ( evt.run_info() && run_info() != evt.run_info() )
	    WARNING( "WriterAscii::write_event: GenEvents contain "
		     "different GenRunInfo objects from - only the "
		     "first such object will be serialized." )
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
      FOREACH (double w, evt.weights())
        m_cursor += sprintf(m_cursor, " %e", w);
      m_cursor += sprintf(m_cursor, "\n");
      flush();
    }

    // Write attributes
    typedef map< string, map<int, shared_ptr<Attribute> > >::value_type value_type1;
    typedef map<int, shared_ptr<Attribute> >::value_type                value_type2;
    FOREACH ( const value_type1& vt1, evt.attributes() ) {
        FOREACH ( const value_type2& vt2, vt1.second ) {

            string st;
            /// @todo This would be nicer as a return value of string & throw exception if there's a conversion problem...
            bool status = vt2.second->to_string(st);

            if( !status ) {
                WARNING( "WriterAscii::write_event: problem serializing attribute: "<<vt1.first )
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

    int vertices_processed = 0;
    int lowest_vertex_id   = 0;

    // Print particles
    FOREACH ( const GenParticlePtr &p, evt.particles() ) {

        // Check to see if we need to write a vertex first
        const GenVertexPtr &v = p->production_vertex();
        int production_vertex = 0;

        if (v) {

            // Check if we need this vertex at all
            if ( v->particles_in().size() > 1 || !v->data().is_zero() ) production_vertex = v->id();
            else if ( v->particles_in().size() == 1 )                   production_vertex = v->particles_in()[0]->id();

            if (production_vertex < lowest_vertex_id) {
                write_vertex(v);
            }

            ++vertices_processed;
            lowest_vertex_id = v->id();
        }

        write_particle( p, production_vertex );
    }

    // Flush rest of the buffer to file
    forced_flush();
}


void WriterAscii::allocate_buffer() {
    if ( m_buffer ) return;
    while( !m_buffer && m_buffer_size >= 256 ) {
        m_buffer = new char[ m_buffer_size ]();
        if (!m_buffer) {
            m_buffer_size /= 2;
            WARNING( "WriterAscii::allocate_buffer: buffer size too large. Dividing by 2. New size: " << m_buffer_size )
        }
    }

    if ( !m_buffer ) {
        ERROR( "WriterAscii::allocate_buffer: could not allocate buffer!" )
        return;
    }

    m_cursor = m_buffer;
}


string WriterAscii::escape(const string s) {
    string ret;
    ret.reserve( s.length()*2 );
    for ( string::const_iterator it = s.begin(); it != s.end(); ++it ) {
        switch ( *it ) {
            case '\\': ret += "\\\\"; break;
            case '\n': ret += "\\|"; break;
            default: ret += *it;
        }
    }
    return ret;
}

void WriterAscii::write_vertex(const GenVertexPtr &v) {

    m_cursor += sprintf( m_cursor, "V %i %i [",v->id(),v->status() );
    flush();

    bool printed_first = false;

    FOREACH( const GenParticlePtr &p, v->particles_in() ) {

        if ( !printed_first ) {
            m_cursor  += sprintf(m_cursor,"%i", p->id());
            printed_first = true;
        }
        else m_cursor += sprintf(m_cursor,",%i",p->id());

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

    typedef map< std::string, shared_ptr<Attribute> >::value_type value_type;
    FOREACH( value_type att, run_info()->attributes() ) {
	string st;
	if ( ! att.second->to_string(st) ) {
	    WARNING ("WriterAscii::write_run_info: problem serializing attribute: "<< att.first )
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

void WriterAscii::write_particle(const GenParticlePtr &p, int second_field) {

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
  // if ( !m_file.is_open() ) return;
  if (ofs && !ofs->is_open()) return;

  forced_flush();
  // m_file << "HepMC::IO_GenEvent-END_EVENT_LISTING" << endl << endl;
  (*m_stream) << "HepMC::IO_GenEvent-END_EVENT_LISTING" << endl << endl;

  // m_file.close();
  if (ofs) ofs->close();
}


} // namespace HepMC
