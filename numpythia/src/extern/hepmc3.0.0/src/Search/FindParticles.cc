// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file FindParticles.cc
 *  @brief Implementation of \b class FindParticles
 *
 */
#include "HepMC/Search/FindParticles.h"
#include "HepMC/Search/FilterList.h"
#include "HepMC/Search/Filter.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

namespace HepMC {


FindParticles::FindParticles(const GenEvent &evt, FilterEvent filter_type, FilterList filter_list) {

    FOREACH( const GenParticlePtr &p, evt.particles() ) {

        if( passed_all_filters(p,filter_list) ) {
            if( filter_type == FIND_LAST ) m_results.clear();

            m_results.push_back( p );

            if( filter_type == FIND_FIRST ) return;
        }
    }
}


FindParticles::FindParticles(const GenParticlePtr &p, FilterParticle filter_type, FilterList filter_list) {

    switch(filter_type) {
        case FIND_ALL_ANCESTORS:
            if( !p->production_vertex() ) break;

            recursive_check_ancestors( p->production_vertex(), filter_list );
            break;
        case FIND_ALL_DESCENDANTS:
            if( !p->end_vertex() ) break;

            recursive_check_descendants( p->end_vertex(), filter_list );
            break;
        case FIND_MOTHERS:
            if( !p->production_vertex() ) break;

            FOREACH( const GenParticlePtr &p_in, p->production_vertex()->particles_in() ) {

                if( passed_all_filters(p_in,filter_list) ) {
                    m_results.push_back( p_in );
                }
            }
            break;
        case FIND_DAUGHTERS:
            if( !p->end_vertex() ) break;

            FOREACH( const GenParticlePtr &p_out, p->end_vertex()->particles_out() ) {

                if( passed_all_filters(p_out,filter_list) ) {
                    m_results.push_back( p_out );
                }
            }
            break;
        case FIND_PRODUCTION_SIBLINGS:
            if( !p->end_vertex() ) break;

            FOREACH( const GenParticlePtr &p_in, p->end_vertex()->particles_in() ) {

                if( passed_all_filters(p_in,filter_list) ) {
                    m_results.push_back( p_in );
                }
            }
            break;
    };
}


FindParticles::FindParticles(const GenVertexPtr &v, FilterParticle filter_type, FilterList filter_list) {

    switch(filter_type) {
        case FIND_ALL_ANCESTORS:
            recursive_check_ancestors( v, filter_list );
            break;
        case FIND_ALL_DESCENDANTS:
            recursive_check_descendants( v, filter_list );
            break;
        case FIND_MOTHERS:
            FOREACH( const GenParticlePtr &p_in, v->particles_in() ) {
                if ( passed_all_filters(p_in,filter_list) ) {
                    m_results.push_back( p_in );
                }
            }
            break;
        case FIND_DAUGHTERS:
            FOREACH( const GenParticlePtr &p_out, v->particles_out() ) {
                if ( passed_all_filters(p_out,filter_list) ) {
                    m_results.push_back( p_out );
                }
            }
            break;
        case FIND_PRODUCTION_SIBLINGS:
        default:
            throw Exception("Invalid filter type provided for FindParticles(GenVertexPtr)");
    };
}


void FindParticles::narrow_down( FilterList filter_list ) {

    int first_null = -1;

    // cost-efficient removing of particles that didn't pass filters
    for(unsigned int i=0; i<m_results.size(); ++i) {

        if( !passed_all_filters(m_results[i],filter_list) ) {

            if( first_null < 0 ) first_null = i;
        }
        else {
            if( first_null >= 0 ) {
                m_results[first_null] = m_results[i];
                ++first_null;
            }
        }
    }

    if( first_null >= 0 ) m_results.resize( first_null );
}

bool FindParticles::passed_all_filters(const GenParticlePtr &p, FilterList &filter_list) {
    if( filter_list.filters().size() == 0 ) return true;

    FOREACH( const Filter &f, filter_list.filters() ) {
        if( f.passed_filter(p) == false ) return false;
    }

    DEBUG( 10, "Filter: passed" )
    return true;
}

void FindParticles::recursive_check_ancestors(const GenVertexPtr &v, FilterList &filter_list) {

    FOREACH( const GenVertexPtr &v_list, m_checked_vertices ) {
        if( v_list->id() == v->id() ) return;
    }

    m_checked_vertices.push_back(v);

    FOREACH( const GenParticlePtr &p_in, v->particles_in() ) {

        if( passed_all_filters(p_in,filter_list) ) {
            m_results.push_back(p_in);
        }

        if( !p_in->production_vertex() ) continue;
        recursive_check_ancestors( p_in->production_vertex(), filter_list );
    }
}

void FindParticles::recursive_check_descendants(const GenVertexPtr &v, FilterList &filter_list) {

    FOREACH( const GenVertexPtr &v_list, m_checked_vertices ) {
        if( v_list->id() == v->id() ) return;
    }

    m_checked_vertices.push_back(v);

    FOREACH( const GenParticlePtr &p_out, v->particles_out() ) {
        if( passed_all_filters(p_out,filter_list) ) {
            m_results.push_back(p_out);
        }

        if( !p_out->end_vertex() ) continue;
        recursive_check_descendants( p_out->end_vertex(), filter_list );
    }
}

} // namespace HepMC
