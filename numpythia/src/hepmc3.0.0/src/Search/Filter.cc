// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file Filter.cc
 *  @brief Implementation of \b class Filter
 *
 */
#include "HepMC/Search/Filter.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"

namespace HepMC {


bool Filter::passed_filter(const GenParticlePtr &p) const {

    switch( m_value_type) {
        case INTEGER_PARAM:   return passed_int_filter (p);
        case BOOL_PARAM:      return passed_bool_filter(p);
        case ATTRIBUTE_PARAM: return passed_attribute_filter(p);
    }

    return false;
}

bool Filter::passed_int_filter(const GenParticlePtr &p ) const {

    int value = 0;

    switch(m_int) {
        case STATUS:          value = p->status();       break;
        case PDG_ID:          value = p->pid();          break;
        case ABS_PDG_ID:      value = abs( p->pid() );   break;
        default:
            // This should never happen
            ERROR( "Unsupported filter ("<<m_int<<")" )
            return false;
    };

    DEBUG( 10, "Filter: checking id="<<p->id()<<" param="<<m_int<<" operator="<<m_operator<<" value="<<value<<" compare to="<<m_int_value )

    switch(m_operator) {
        case EQUAL:            return (value==m_int_value);
        case GREATER:          return (value> m_int_value);
        case LESS:             return (value< m_int_value);
        case GREATER_OR_EQUAL: return (value>=m_int_value);
        case LESS_OR_EQUAL:    return (value<=m_int_value);
        case NOT_EQUAL:        return (value!=m_int_value);
    };

    return false;
}

bool Filter::passed_bool_filter(const GenParticlePtr &p ) const {

    bool         result = false;
    GenVertexPtr buf;

    DEBUG( 10, "Filter: checking id="<<p->id()<<" param="<<m_bool<<" value="<<m_bool_value<<" (bool)" )

    switch( m_bool ) {
        case HAS_END_VERTEX:           result = (bool) p->end_vertex();        break;
        case HAS_PRODUCTION_VERTEX:    result = (bool) p->production_vertex(); break;
        case HAS_SAME_PDG_ID_DAUGHTER:
            buf = p->end_vertex();
            if( !buf ) {
                result = false;
                break;
            }

            if( buf->particles_out().size() == 0 ) {
                result = false;
                break;
            }

            FOREACH( const GenParticlePtr &p_out, buf->particles_out() ) {

                if( p_out->pid() == p->pid() ) {
                    result = true;
                    break;
                }
            }

            break;
        case IS_STABLE: result = (bool) (p->status() == 1); break;
        case IS_BEAM:   result = (bool) (p->status() == 4); break;
    };

    if( m_bool_value == false ) result = !result;

    return result;
}

bool Filter::passed_attribute_filter(const GenParticlePtr &p ) const {

    bool ret = false;

    string attribute = p->attribute_as_string(m_attribute_name);

    DEBUG( 10, "Filter: checking id="<<p->id()<<" m_attribute="<<m_attribute<<" m_bool_val="<<m_bool_value<<" att name='"<<m_attribute_name<<"' att str='"<<m_attribute_str<<"' compare to='"<<attribute<<"'"  )

    switch( m_attribute ) {
        case ATTRIBUTE_EXISTS:   ret = (attribute.length() > 0);                  break;
        case ATTRIBUTE_IS_EQUAL: ret = (m_attribute_str.compare(attribute) == 0); break;
    }

    if( !m_bool_value ) ret = !ret;

    return ret;
}

} // namespace HepMC
