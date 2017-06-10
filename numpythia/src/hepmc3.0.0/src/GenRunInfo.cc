// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file GenRunInfo.cc
 *  @brief Implementation of \b class GenRunInfo
 *
 */
#include "HepMC/GenRunInfo.h"
#include "HepMC/Data/GenRunInfoData.h"
#include <sstream>

namespace HepMC {


void GenRunInfo::set_weight_names(const std::vector<std::string> & names) {
    m_weight_indices.clear();
    m_weight_names = names;
    for ( int i = 0, N = names.size(); i < N; ++i ) {
	string name = names[i];
	if ( name.empty() ) {
	    std::ostringstream oss;
	    oss << i;
	    name = oss.str();
	    m_weight_names[i] = name;
	}
	if ( has_weight(name) )
	    throw std::logic_error("GenRunInfo::set_weight_names: "
				   "Duplicate weight name '" + name +
				   "' found.");
	m_weight_indices[name] = i;
    }
}

string GenRunInfo::attribute_as_string(const string &name) const {

    std::map< std::string, shared_ptr<Attribute> >::iterator i = m_attributes.find(name);
    if( i == m_attributes.end() ) return string();

    if( !i->second ) return string();

    string ret;
    i->second->to_string(ret);

    return ret;
}

void GenRunInfo::write_data(GenRunInfoData& data) const {

    // Weight names
    data.weight_names = this->weight_names();

    // Attributes
    typedef std::map<std::string, shared_ptr<Attribute> >::value_type att_val_t;

    FOREACH( const att_val_t& vt, this->attributes() ) {
        std::string att;
        vt.second->to_string(att);

        data.attribute_name.  push_back(vt.first);
        data.attribute_string.push_back(att);
    }

    // Tools
    FOREACH( const ToolInfo &tool, this->tools() ) {
        data.tool_name.       push_back(tool.name);
        data.tool_version.    push_back(tool.version);
        data.tool_description.push_back(tool.description);
    }
}

void GenRunInfo::read_data(const GenRunInfoData& data) {

    //this->clear();

    // Weight names
    set_weight_names(data.weight_names);

    // Attributes
    for(unsigned int i=0; i<data.attribute_name.size(); ++i) {
        add_attribute( data.attribute_name[i],
                       make_shared<StringAttribute>(data.attribute_string[i]) );
    }

    // Tools
    for(unsigned int i=0; i<data.tool_name.size(); ++i) {
        ToolInfo ti;
        ti.name        = data.tool_name[i];
        ti.version     = data.tool_version[i];
        ti.description = data.tool_description[i];

        this->tools().push_back(ti);
    }
}
} // namespace HepMC
