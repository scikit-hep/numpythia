// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file HEPEVT_Wrapper.cc
 *  @brief Implementation of conversion functions for HEPEVT block
 ***********************************************************************
 * Some parts from HepMC2 library
 * Matt.Dobbs@Cern.CH, January 2000
 * HEPEVT IO class
 ***********************************************************************
 *
 */
#ifndef HEPEVT_WRAPPER_HEADER_ONLY
#include "HepMC3/HEPEVT_Wrapper.h"
#include "HepMC3/GenEvent.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenVertex.h"
#include <algorithm>
#include <set>
#include <vector>
namespace HepMC3
{

struct HEPEVT*  hepevtptr;


/** @brief comparison of two particles */
struct GenParticlePtr_greater_order
{
    /** @brief comparison of two particles */
    bool operator()(ConstGenParticlePtr lx, ConstGenParticlePtr rx ) const
    {
        /* Cannot use id as it could be different*/
        if (lx->pid() !=rx->pid()) return (lx->pid() < rx->pid());
        if (lx->status() !=rx->status()) return (lx->status() < rx->status());
        /*Hopefully it will reach this point not too ofter.*/
        return (lx->momentum().e()<rx->momentum().e());

    }
};
/** @brief  Order vertices with equal paths. */
struct pair_GenVertexPtr_int_greater
{
    /** @brief  Order vertices with equal paths. If the paths are equal, order in other quantities.
     * We cannot use id, as it can be assigned in different way*/
    bool operator()( const std::pair<ConstGenVertexPtr,int>& lx, const std::pair<ConstGenVertexPtr,int>& rx ) const
    {
        if (lx.second!=rx.second) return (lx.second < rx.second);
        if (lx.first->particles_in().size()!=rx.first->particles_in().size()) return (lx.first->particles_in().size()<rx.first->particles_in().size());
        if (lx.first->particles_out().size()!=rx.first->particles_out().size()) return (lx.first->particles_out().size()<rx.first->particles_out().size());
        /* The code below is usefull mainly for debug. Assures strong ordering.*/
        std::vector<int> lx_id_in;
        std::vector<int> rx_id_in;
        for (ConstGenParticlePtr pp: lx.first->particles_in()) lx_id_in.push_back(pp->pid());
        for (ConstGenParticlePtr pp: rx.first->particles_in()) rx_id_in.push_back(pp->pid());
        std::sort(lx_id_in.begin(),lx_id_in.end());
        std::sort(rx_id_in.begin(),rx_id_in.end());
        for (unsigned int i=0; i<lx_id_in.size(); i++) if (lx_id_in[i]!=rx_id_in[i]) return  (lx_id_in[i]<rx_id_in[i]);

        std::vector<int> lx_id_out;
        std::vector<int> rx_id_out;
        for (ConstGenParticlePtr pp: lx.first->particles_in()) lx_id_out.push_back(pp->pid());
        for (ConstGenParticlePtr pp: rx.first->particles_in()) rx_id_out.push_back(pp->pid());
        std::sort(lx_id_out.begin(),lx_id_out.end());
        std::sort(rx_id_out.begin(),rx_id_out.end());
        for (unsigned int i=0; i<lx_id_out.size(); i++) if (lx_id_out[i]!=rx_id_out[i]) return  (lx_id_out[i]<rx_id_out[i]);

        std::vector<double> lx_mom_in;
        std::vector<double> rx_mom_in;
        for (ConstGenParticlePtr pp: lx.first->particles_in()) lx_mom_in.push_back(pp->momentum().e());
        for (ConstGenParticlePtr pp: rx.first->particles_in()) rx_mom_in.push_back(pp->momentum().e());
        std::sort(lx_mom_in.begin(),lx_mom_in.end());
        std::sort(rx_mom_in.begin(),rx_mom_in.end());
        for (unsigned int i=0; i<lx_mom_in.size(); i++) if (lx_mom_in[i]!=rx_mom_in[i]) return  (lx_mom_in[i]<rx_mom_in[i]);

        std::vector<double> lx_mom_out;
        std::vector<double> rx_mom_out;
        for (ConstGenParticlePtr pp: lx.first->particles_in()) lx_mom_out.push_back(pp->momentum().e());
        for (ConstGenParticlePtr pp: rx.first->particles_in()) rx_mom_out.push_back(pp->momentum().e());
        std::sort(lx_mom_out.begin(),lx_mom_out.end());
        std::sort(rx_mom_out.begin(),rx_mom_out.end());
        for (unsigned int i=0; i<lx_mom_out.size(); i++) if (lx_mom_out[i]!=rx_mom_out[i]) return  (lx_mom_out[i]<rx_mom_out[i]);
        /* The code above is usefull mainly for debug*/

        return (lx.first<lx.first); /*This  is random. This should never happen*/
    }
};
/** @brief Calculates the path to the top (beam) particles */
void calculate_longest_path_to_top(ConstGenVertexPtr v,std::map<ConstGenVertexPtr,int>& pathl)
{
    int p=0;
    for(ConstGenParticlePtr pp: v->particles_in()) {
        ConstGenVertexPtr v2 = pp->production_vertex();
        if (v2==v) continue; //LOOP! THIS SHOULD NEVER HAPPEN FOR A PROPER EVENT!
        if (!v2) p=std::max(p,1);
        else
        {if (pathl.find(v2)==pathl.end())  calculate_longest_path_to_top(v2,pathl); p=std::max(p, pathl[v2]+1);}
    }
    pathl[v]=p;
    return;
}


bool HEPEVT_Wrapper::HEPEVT_to_GenEvent( GenEvent* evt )
{
    if ( !evt ) { std::cerr << "IO_HEPEVT::fill_next_event error  - passed null event." << std::endl; return false;}
    evt->set_event_number( HEPEVT_Wrapper::event_number());
    std::map<GenParticlePtr,int > hepevt_particles;
    std::map<int,GenParticlePtr > particles_index;
    std::map<GenVertexPtr,std::pair<std::set<int>,std::set<int> > > hepevt_vertices;
    std::map<int,GenVertexPtr > vertex_index;
    for ( int i = 1; i <= HEPEVT_Wrapper::number_entries(); i++ )
    {
        GenParticlePtr p=make_shared<GenParticle>();
        p->set_momentum(FourVector( HEPEVT_Wrapper::px(i), HEPEVT_Wrapper::py(i), HEPEVT_Wrapper::pz(i), HEPEVT_Wrapper::e(i) ));
        p->set_status(HEPEVT_Wrapper::status(i));
        p->set_pid(HEPEVT_Wrapper::id(i)); //Confusing!
        p->set_generated_mass( HEPEVT_Wrapper::m(i));
        hepevt_particles[p]=i;
        particles_index[i]=p;
        GenVertexPtr v=make_shared<GenVertex>();
        v->set_position(FourVector( HEPEVT_Wrapper::x(i), HEPEVT_Wrapper::y(i), HEPEVT_Wrapper::z(i), HEPEVT_Wrapper::t(i)));
        v->add_particle_out(p);
        std::set<int> in;
        std::set<int> out;
        out.insert(i);
        vertex_index[i]=v;
        hepevt_vertices[v]=std::pair<std::set<int>,std::set<int> >(in,out);
    }
    /* The part above is always correct as it is a raw information without any topology.*/

    /* In this way we trust mother information TODO: implement "Trust daughters"*/
    for (std::map<GenParticlePtr,int >::iterator it1= hepevt_particles.begin(); it1!= hepevt_particles.end(); ++it1)
        for (std::map<GenParticlePtr,int >::iterator it2= hepevt_particles.begin(); it2!= hepevt_particles.end(); ++it2)
            if   (HEPEVT_Wrapper::first_parent(it2->second)<=it1->second&&it1->second<=HEPEVT_Wrapper::last_parent(it2->second)) /*I'm you father, Luck!*/
                hepevt_vertices[it2->first->production_vertex()].first.insert(it1->second);
    /* Now all incoming sets are correct for all vertices. But we have duplicates.*/

    /* Disconnect all particles from the vertices*/
    for ( int i = 1; i <= HEPEVT_Wrapper::number_entries(); i++ ) vertex_index[i]->remove_particle_out(particles_index[i]);

    /*Fill container with vertices with unique sets of incoming particles. Merge the outcoming particle sets.*/
    std::map<GenVertexPtr,std::pair<std::set<int>,std::set<int> > > final_vertices_map;
    for (std::map<GenVertexPtr,std::pair<std::set<int>,std::set<int> > >::iterator vs= hepevt_vertices.begin(); vs!= hepevt_vertices.end(); ++vs)
    {
        if ((final_vertices_map.size()==0)||(vs->second.first.size()==0&&vs->second.second.size()!=0)) { final_vertices_map.insert(*vs);  continue; } /*Always insert particles out of nowhere*/
        std::map<GenVertexPtr,std::pair<std::set<int>,std::set<int> > >::iterator  v2;
        for (v2=final_vertices_map.begin(); v2!=final_vertices_map.end(); ++v2) if (vs->second.first==v2->second.first) {v2->second.second.insert(vs->second.second.begin(),vs->second.second.end()); break;}
        if (v2==final_vertices_map.end()) final_vertices_map.insert(*vs);
    }

    std::vector<GenParticlePtr> final_particles;
    std::set<int> used;
    for (std::map<GenVertexPtr,std::pair<std::set<int>,std::set<int> > >:: iterator it=final_vertices_map.begin(); it!=final_vertices_map.end(); ++it)
    {
        GenVertexPtr v=it->first;
        std::set<int> in=it->second.first;
        std::set<int> out=it->second.second;
        used.insert(in.begin(),in.end());
        used.insert(out.begin(),out.end());
        for (std::set<int>::iterator el=in.begin(); el!=in.end(); ++el) v->add_particle_in(particles_index[*el]);
        if (in.size()!=0) for (std::set<int>::iterator el=out.begin(); el!=out.end(); ++el) v->add_particle_out(particles_index[*el]);
    }
    for (std::set<int>::iterator el=used.begin(); el!=used.end(); ++el) final_particles.push_back(particles_index[*el]);
    /* One can put here a check on the number of particles/vertices*/

    evt->add_tree(final_particles);

    return true;
}


bool HEPEVT_Wrapper::GenEvent_to_HEPEVT( const GenEvent* evt )
{
    /// This writes an event out to the HEPEVT common block. The daughters
    /// field is NOT filled, because it is possible to contruct graphs
    /// for which the mothers and daughters cannot both be make sequential.
    /// This is consistent with how pythia fills HEPEVT (daughters are not
    /// necessarily filled properly) and how IO_HEPEVT reads HEPEVT.
    //
    if ( !evt ) return false;

    /*AV Sorting the vertices by the lengths of their longest incoming paths assures the mothers will not go before the daughters*/
    /* Calculate all paths*/
    std::map<ConstGenVertexPtr,int> longest_paths;
    for (ConstGenVertexPtr v: evt->vertices()) calculate_longest_path_to_top(v,longest_paths);
    /* Sort paths*/
    std::vector<std::pair<ConstGenVertexPtr,int> > sorted_paths;
    copy(longest_paths.begin(),longest_paths.end(),std::back_inserter(sorted_paths));
    sort(sorted_paths.begin(),sorted_paths.end(),pair_GenVertexPtr_int_greater());

    std::vector<ConstGenParticlePtr> sorted_particles;
    std::vector<ConstGenParticlePtr> stable_particles;
    /*For a valid "Trust mothers" HEPEVT record we must  keep mothers together*/
    for (std::pair<ConstGenVertexPtr,int> it: sorted_paths)
    {
        std::vector<ConstGenParticlePtr> Q = it.first->particles_in();
        sort(Q.begin(),Q.end(),GenParticlePtr_greater_order());
        copy(Q.begin(),Q.end(),std::back_inserter(sorted_particles));
        /*For each vertex put all outgoing particles w/o end vertex. Ordering of particles to produces reproduceable record*/
        for(ConstGenParticlePtr pp: it.first->particles_out())
            if(!(pp->end_vertex())) stable_particles.push_back(pp);
    }
    sort(stable_particles.begin(),stable_particles.end(),GenParticlePtr_greater_order());
    copy(stable_particles.begin(),stable_particles.end(),std::back_inserter(sorted_particles));

    int particle_counter=std::min(int(sorted_particles.size()),HEPEVT_Wrapper::max_number_entries());
    /* fill the HEPEVT event record (MD code)*/
    HEPEVT_Wrapper::set_event_number( evt->event_number() );
    HEPEVT_Wrapper::set_number_entries( particle_counter );
    for ( int i = 1; i <= particle_counter; ++i )
    {
        HEPEVT_Wrapper::set_status( i, sorted_particles[i-1]->status() );
        HEPEVT_Wrapper::set_id( i, sorted_particles[i-1]->pid() );
        FourVector m = sorted_particles[i-1]->momentum();
        HEPEVT_Wrapper::set_momentum( i, m.px(), m.py(), m.pz(), m.e() );
        HEPEVT_Wrapper::set_mass( i, sorted_particles[i-1]->generated_mass() );
        // there should ALWAYS be particles in any vertex, but some generators
        // are making non-kosher HepMC events
        if ( sorted_particles[i-1]->production_vertex() &&
                sorted_particles[i-1]->production_vertex()->particles_in().size())
        {
            FourVector p = sorted_particles[i-1]->production_vertex()->position();
            HEPEVT_Wrapper::set_position( i, p.x(), p.y(), p.z(), p.t() );
            std::vector<int> mothers;
            mothers.clear();

            for(ConstGenParticlePtr it: sorted_particles[i-1]->production_vertex()->particles_in())
                for ( int j = 1; j <= particle_counter; ++j )
                    if (sorted_particles[j-1]==(it))
                        mothers.push_back(j);
            sort(mothers.begin(),mothers.end());
            if (mothers.size()==0)
                mothers.push_back(0);
            if (mothers.size()==1) mothers.push_back(mothers[0]);

            HEPEVT_Wrapper::set_parents( i, mothers.front(), mothers.back() );
        }
        else
        {
            HEPEVT_Wrapper::set_position( i, 0, 0, 0, 0 );
            HEPEVT_Wrapper::set_parents( i, 0, 0 );
        }
        HEPEVT_Wrapper::set_children( i, 0, 0 );
    }
    return true;
}
}
#endif
