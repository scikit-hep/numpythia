// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file ReaderAsciiHepMC2.cc
 *  @brief Implementation of \b class ReaderAsciiHepMC2
 *
 */
#include "HepMC/ReaderAsciiHepMC2.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenHeavyIon.h"
#include "HepMC/GenPdfInfo.h"
#include "HepMC/Setup.h"

#include <cstring>
#include <cstdlib>

namespace HepMC {

ReaderAsciiHepMC2::ReaderAsciiHepMC2(const std::string& filename):
m_file(filename) {
    if( !m_file.is_open() ) {
        ERROR( "ReaderAsciiHepMC2: could not open input file: "<<filename )
    }
    set_run_info(make_shared<GenRunInfo>());
}

bool ReaderAsciiHepMC2::read_event(GenEvent &evt) {
    char          buf[512];
    bool          parsed_event_header            = false;
    bool          is_parsing_successful          = true;
    int           parsing_result                 = 0;
    unsigned int  vertices_count                 = 0;
    unsigned int  current_vertex_particles_count = 0;
    unsigned int  current_vertex_particles_parsed= 0;

    evt.clear();
    evt.set_run_info(run_info());

    // Empty cache
    m_vertex_cache.clear();
    m_vertex_barcodes.clear();

    m_particle_cache.clear();
    m_end_vertex_barcodes.clear();

    //
    // Parse event, vertex and particle information
    //
    while(!failed()) {
        m_file.getline(buf,512);

        if( strlen(buf) == 0 ) continue;

        // Check for IO_GenEvent header/footer
        if( strncmp(buf,"HepMC",5) == 0 ) {
            if(parsed_event_header) {
                is_parsing_successful = true;
                break;
            }
            continue;
        }

        switch(buf[0]) {
            case 'E':
                parsing_result = parse_event_information(evt,buf);
                if(parsing_result<0) {
                    is_parsing_successful = false;
                    ERROR( "ReaderAsciiHepMC2: error parsing event information" )
                }
                else {
                    vertices_count = parsing_result;
                    m_vertex_cache.reserve(vertices_count);
                    m_particle_cache.reserve(vertices_count*3);
                    m_vertex_barcodes.reserve(vertices_count);
                    m_end_vertex_barcodes.reserve(vertices_count*3);
                    is_parsing_successful = true;
                }
                parsed_event_header = true;
                break;
            case 'V':
                // If starting new vertex: verify if previous was fully parsed

                /** @bug HepMC2 files produced with Pythia8 are known to have wrong
                         information about number of particles in vertex. Hence '<' sign */
                if(current_vertex_particles_parsed < current_vertex_particles_count) {
                    is_parsing_successful = false;
                    break;
                }
                current_vertex_particles_parsed = 0;

                parsing_result = parse_vertex_information(buf);

                if(parsing_result<0) {
                    is_parsing_successful = false;
                    ERROR( "ReaderAsciiHepMC2: error parsing vertex information" )
                }
                else {
                    current_vertex_particles_count = parsing_result;
                    is_parsing_successful = true;
                }
                break;
            case 'P':

                parsing_result   = parse_particle_information(buf);

                if(parsing_result<0) {
                    is_parsing_successful = false;
                    ERROR( "ReaderAsciiHepMC2: error parsing particle information" )
                }
                else {
                    ++current_vertex_particles_parsed;
                    is_parsing_successful = true;
                }
                break;
            case 'U':
                is_parsing_successful = parse_units(evt,buf);
                break;
            case 'F':
                is_parsing_successful = parse_pdf_info(evt,buf);
                break;
            case 'H':
                is_parsing_successful = parse_heavy_ion(evt,buf);
                break;
            case 'N':
                is_parsing_successful = parse_weight_names(buf);
                break;
            case 'C':
                is_parsing_successful = parse_xs_info(evt,buf);
                break;            
            default:
                WARNING( "ReaderAsciiHepMC2: skipping unrecognised prefix: " << buf[0] )
                is_parsing_successful = true;
                break;
        }

        if( !is_parsing_successful ) break;

        // Check for next event
        buf[0] = m_file.peek();
        if( parsed_event_header && buf[0]=='E' ) break;
    }

    // Check if all particles in last vertex were parsed

    /** @bug HepMC2 files produced with Pythia8 are known to have wrong
             information about number of particles in vertex. Hence '<' sign */
    if( is_parsing_successful && current_vertex_particles_parsed < current_vertex_particles_count ) {
        ERROR( "ReaderAsciiHepMC2: not all particles parsed" )
        is_parsing_successful = false;
    }
    // Check if all vertices were parsed
    else if( is_parsing_successful && m_vertex_cache.size() != vertices_count ) {
        ERROR( "ReaderAsciiHepMC2: not all vertices parsed" )
        is_parsing_successful = false;
    }

    if( !is_parsing_successful ) {
        ERROR( "ReaderAsciiHepMC2: event parsing failed. Returning empty event" )
        DEBUG( 1, "Parsing failed at line:" << std::endl << buf )

        evt.clear();
        m_file.clear(std::ios::badbit);

        return 0;
    }

    // Restore production vertex pointers
    for(unsigned int i=0; i<m_particle_cache.size(); ++i) {
        if( !m_end_vertex_barcodes[i] ) continue;

        for(unsigned int j=0; j<m_vertex_cache.size(); ++j) {
            if( m_vertex_barcodes[j] == m_end_vertex_barcodes[i] ) {
                m_vertex_cache[j]->add_particle_in(m_particle_cache[i]);
                break;
            }
        }
    }

    // Remove vertices with no incoming particles or no outgoing particles
    for(unsigned int i=0; i<m_vertex_cache.size(); ++i) {
        if( m_vertex_cache[i]->particles_in().size() == 0 ) {
            m_vertex_cache[i] = NULL;
        }
        else if( m_vertex_cache[i]->particles_out().size() == 0 ) {
            m_vertex_cache[i] = NULL;
        }
    }

    // Reserve memory for the event
    evt.reserve( m_particle_cache.size(), m_vertex_cache.size() );

    // Add whole event tree in topological order
    evt.add_tree( m_particle_cache );

    return 1;
}

int ReaderAsciiHepMC2::parse_event_information(GenEvent &evt, const char *buf) {
    const char          *cursor             = buf;
    int                  event_no           = 0;
    int                  vertices_count     = 0;
    int                  random_states_size = 0;
    int                  weights_size       = 0;
    std::vector<long>    random_states(0);
    std::vector<double>  weights(0);

    // event number
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    event_no = atoi(cursor);
    evt.set_event_number(event_no);

    // SKIPPED: mpi
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: event scale
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: alpha_qcd
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: alpha_qed
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: signal_process_id
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: signal_process_vertex
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // num_vertices
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    vertices_count = atoi(cursor);

    // SKIPPED: beam 1
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: beam 2
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: random states
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    random_states_size = atoi(cursor);
    random_states.resize(random_states_size);

    for ( int i = 0; i < random_states_size; ++i ) {
        if( !(cursor = strchr(cursor+1,' ')) ) return -1;
        random_states[i] = atoi(cursor);
    }

    // weights
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    weights_size = atoi(cursor);
    weights.resize(weights_size);

    for ( int i = 0; i < weights_size; ++i ) {
        if( !(cursor = strchr(cursor+1,' ')) ) return -1;
        weights[i] = atof(cursor);
    }

    evt.weights() = weights;

    DEBUG( 10, "ReaderAsciiHepMC2: E: "<<event_no<<" ("<<vertices_count<<"V, "<<weights_size<<"W, "<<random_states_size<<"RS)" )

    return vertices_count;
}

bool ReaderAsciiHepMC2::parse_units(GenEvent &evt, const char *buf) {
    const char *cursor  = buf;

    // momentum
    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    ++cursor;
    Units::MomentumUnit momentum_unit = Units::momentum_unit(cursor);

    // length
    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    ++cursor;
    Units::LengthUnit length_unit = Units::length_unit(cursor);

    evt.set_units(momentum_unit,length_unit);

    DEBUG( 10, "ReaderAsciiHepMC2: U: " << Units::name(evt.momentum_unit()) << " " << Units::name(evt.length_unit()) )

    return true;
}

int ReaderAsciiHepMC2::parse_vertex_information(const char *buf) {
    GenVertexPtr  data = make_shared<GenVertex>();
    FourVector    position;
    const char   *cursor            = buf;
    int           barcode           = 0;
    int           num_particles_out = 0;

    // barcode
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    barcode = atoi(cursor);

    // SKIPPED: id
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // x
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    position.setX(atof(cursor));

    // y
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    position.setY(atof(cursor));

    // z
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    position.setZ(atof(cursor));

    // t
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    position.setT(atof(cursor));
    data->set_position( position );

    // SKIPPED: num_orphans_in
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // num_particles_out
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    num_particles_out = atoi(cursor);

    // SKIPPING: weights_size, weights

    // Add original vertex barcode to the cache
    m_vertex_cache.push_back( data );
    m_vertex_barcodes.push_back( barcode );

    DEBUG( 10, "ReaderAsciiHepMC2: V: "<<-(int)m_vertex_cache.size()<<" (old barcode"<<barcode<<") "<<num_particles_out<<" particles)" )

    return num_particles_out;
}

int ReaderAsciiHepMC2::parse_particle_information(const char *buf) {
    GenParticlePtr  data = make_shared<GenParticle>();
    FourVector      momentum;
    const char     *cursor  = buf;
    int             end_vtx = 0;

    /// @todo barcode ignored but maybe should be put as an attribute?...
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // id
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    data->set_pid( atoi(cursor) );

    // px
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    momentum.setPx(atof(cursor));

    // py
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    momentum.setPy(atof(cursor));

    // pz
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    momentum.setPz(atof(cursor));

    // pe
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    momentum.setE(atof(cursor));
    data->set_momentum(momentum);

    // m
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    data->set_generated_mass( atof(cursor) );

    // status
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    data->set_status( atoi(cursor) );

    // SKIPPED: theta
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // SKIPPED: phi
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;

    // end_vtx_code
    if( !(cursor = strchr(cursor+1,' ')) ) return -1;
    end_vtx = atoi(cursor);

    // SKIPPING: flow_size, flow patterns

    // Set prod_vtx link
    if( end_vtx == m_vertex_barcodes.back() ) {
        m_vertex_cache.back()->add_particle_in(data);
        end_vtx = 0;
    }
    else {
        m_vertex_cache.back()->add_particle_out(data);
    }

    m_particle_cache.push_back( data );
    m_end_vertex_barcodes.push_back( end_vtx );

    DEBUG( 10, "ReaderAsciiHepMC2: P: "<<m_particle_cache.size()<<" ( pid: "<<data->pid()<<") end vertex: "<<end_vtx )

    return 0;
}

bool ReaderAsciiHepMC2::parse_xs_info(GenEvent &evt, const char *buf) {
    const char *cursor  = buf;
    shared_ptr<GenCrossSection>  xs     = make_shared<GenCrossSection>();

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    double xs_val  = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    double xs_err = atof(cursor);

    xs->set_cross_section( xs_val , xs_err);
    evt.add_attribute("GenCrossSection",xs);
   
    return true;
}

bool ReaderAsciiHepMC2::parse_weight_names(const char *buf) {
    const char     *cursor  = buf;
    const char     *cursor2 = buf;
    int             w_count = 0;
    vector<string>  w_names;

    // Ignore weight names if no GenRunInfo object
    if( !run_info() ) return true;

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    w_count = atoi(cursor);

    if( w_count <= 0 ) return false;

    w_names.resize(w_count);

    for( int i=0; i < w_count; ++i ) {
        // Find pair of '"' characters
        if( !(cursor  = strchr(cursor+1,'"')) ) return false;
        if( !(cursor2 = strchr(cursor+1,'"')) ) return false;

        // Strip cursor of leading '"' character
        ++cursor;

        w_names[i].assign(cursor, cursor2-cursor);

        cursor = cursor2;
    }

    run_info()->set_weight_names(w_names);

    return true;
}

bool ReaderAsciiHepMC2::parse_heavy_ion(GenEvent &evt, const char *buf) {
    shared_ptr<GenHeavyIon>  hi     = make_shared<GenHeavyIon>();
    const char              *cursor = buf;

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Ncoll_hard = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Npart_proj = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Npart_targ = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Ncoll = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->spectator_neutrons = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->spectator_protons = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->N_Nwounded_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Nwounded_N_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->Nwounded_Nwounded_collisions = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->impact_parameter = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->event_plane_angle = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->eccentricity = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    hi->sigma_inel_NN = atof(cursor);

    // Not in HepMC2:
    hi->centrality = 0.0;

    evt.add_attribute("GenHeavyIon",hi);

    return true;
}

bool ReaderAsciiHepMC2::parse_pdf_info(GenEvent &evt, const char *buf) {
    shared_ptr<GenPdfInfo>  pi     = make_shared<GenPdfInfo>();
    const char             *cursor = buf;

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->parton_id[0] = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->parton_id[1] = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->x[0] = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->x[1] = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->scale = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->xf[0] = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->xf[1] = atof(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->pdf_id[0] = atoi(cursor);

    if( !(cursor = strchr(cursor+1,' ')) ) return false;
    pi->pdf_id[1] = atoi(cursor);

    evt.add_attribute("GenPdfInfo",pi);

    return true;
}

void ReaderAsciiHepMC2::close() {
    if( !m_file.is_open() ) return;
    m_file.close();
}

} // namespace HepMC
