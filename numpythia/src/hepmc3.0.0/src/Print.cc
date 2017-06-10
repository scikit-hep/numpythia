// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
///
/// @file Print.cc
/// @brief Implementation of static \b class Print
///
/// @todo AB: Why a static class rather than some namespaced free functions?
///
#include "HepMC/Print.h"
#include "HepMC/Attribute.h"

using namespace std;

namespace HepMC {


void Print::content( const GenEvent &event ) {
    cout<<"--------------------------------"<<endl;
    cout<<"--------- EVENT CONTENT --------"<<endl;
    cout<<"--------------------------------"<<endl;
    cout<<endl;

    cout<<"Weights (" << event.weights().size() <<"): "<<endl;
    FOREACH( const double w, event.weights() ) {
        cout << w << endl;
    }

    cout<<"Attributes:"<<endl;

    typedef map< string, map<int, shared_ptr<Attribute> > >::value_type value_type1;
    typedef map<int, shared_ptr<Attribute> >::value_type                value_type2;

    FOREACH( const value_type1& vt1, event.attributes() ) {
        FOREACH( const value_type2& vt2, vt1.second ) {
            cout << vt2.first << ": " << vt1.first << endl;
        }
    }

    cout<<"GenParticlePtr ("<<event.particles().size()<<")"<<endl;

    FOREACH( const GenParticlePtr &p, event.particles() )
    {
        HepMC::Print::line(p);
    }

    cout<<"GenVertexPtr ("<<event.vertices().size()<<")"<<endl;
    FOREACH( const GenVertexPtr &v, event.vertices() ) {
        HepMC::Print::line(v);
    }

    cout<<"-----------------------------"<<endl;
}

void Print::listing( const GenEvent &event, unsigned short precision ) {

    // Find the current stream state
    ios_base::fmtflags orig = cout.flags();
    streamsize         prec = cout.precision();

    // Set precision
    cout.precision( precision );

    cout << "________________________________________________________________________" << endl;
    cout << "GenEvent: #" << event.event_number() << endl;
    cout << " Momentum units: " << Units::name(event.momentum_unit())
         << " Position units: " << Units::name(event.length_unit()) << endl;
    cout << " Entries in this event: " << event.vertices().size() << " vertices, "
         << event.particles().size() << " particles, "
         << event.weights().size()   << " weights." << endl;

    const FourVector &pos = event.event_pos();
    cout << " Position offset: " << pos.x() << ", " << pos.y() << ", " << pos.z() << ", " << pos.t() << endl;

    // Print a legend to describe the particle info
    cout << "                                    GenParticle Legend" << endl;
    cout << "         ID    PDG ID   "
         << "( px,       py,       pz,     E )"
         << "   Stat ProdVtx" << endl;
    cout << "________________________________________________________________________" << endl;

    // Print all vertices
    FOREACH( const GenVertexPtr &v, event.vertices() ) {
        HepMC::Print::listing(v);
    }

    // Restore the stream state
    cout.flags(orig);
    cout.precision(prec);
    cout << "________________________________________________________________________" << endl;
}

void Print::listing( const GenVertexPtr &v ) {
    cout << "Vtx: ";
    cout.width(6);
    cout << v->id() << " stat: ";
    cout.width(3);
    cout << v->status();

    const FourVector &pos = v->position();
    if( !pos.is_zero() ) {
        cout << " (X,cT): " << pos.x()<<" "<<pos.y()<<" "<<pos.z()<<" "<<pos.t();
    }
    else cout << " (X,cT): 0";

    cout << endl;

    bool printed_header = false;

    // Print out all the incoming particles
    FOREACH( const GenParticlePtr &p, v->particles_in() ) {
        if( !printed_header ) {
            cout << " I: ";
            printed_header = true;
        }
        else cout << "    ";

        HepMC::Print::listing(p);
    }

    printed_header = false;

    // Print out all the outgoing particles
    FOREACH( const GenParticlePtr &p, v->particles_out() ) {
        if( !printed_header ) {
            cout << " O: ";
            printed_header = true;
        }
        else cout << "    ";

        HepMC::Print::listing(p);
    }
}

void Print::listing( const GenParticlePtr &p ) {
    cout << " ";
    cout.width(6);
    cout << p->id();
    cout.width(9);
    cout << p->pid() << " ";
    cout.width(9);
    cout.setf(ios::scientific, ios::floatfield);
    cout.setf(ios_base::showpos);

    const FourVector &momentum = p->momentum();

    cout.width(9);
    cout << momentum.px() << ",";
    cout.width(9);
    cout << momentum.py() << ",";
    cout.width(9);
    cout << momentum.pz() << ",";
    cout.width(9);
    cout << momentum.e() << " ";
    cout.setf(ios::fmtflags(0), ios::floatfield);
    cout.unsetf(ios_base::showpos);
    cout.width(3);
    cout << p->status();

    const GenVertexPtr prod = p->production_vertex();

    if( prod ) {
        cout.width(6);
        cout << prod->id();
    }

    cout << endl;
}

void Print::line(const GenVertexPtr &v) {
    cout << "GenVertex:  " << v->id() << " stat: ";
    cout.width(3);
    cout << v->status();
    cout << " in: "  << v->particles_in().size();
    cout.width(3);
    cout << " out: " << v->particles_out().size();

    const FourVector &pos = v->position();
    cout << " has_set_position: ";
    if( v->has_set_position() ) cout << "true";
    else                        cout << "false";

    cout << " (X,cT): " << pos.x()<<", "<<pos.y()<<", "<<pos.z()<<", "<<pos.t() << endl;
}

void Print::line(const GenParticlePtr &p) {

    cout << "GenParticle: ";
    cout.width(3);
    cout << p->id() <<" PDGID: ";
    cout.width(5);
    cout << p->pid();

    // Find the current stream state
    ios_base::fmtflags orig = cout.flags();

    cout.setf(ios::scientific, ios::floatfield);
    cout.setf(ios_base::showpos);
    streamsize prec = cout.precision();

    // Set precision
    cout.precision( 2 );

    const FourVector &momentum = p->momentum();

    cout << " (P,E)=" << momentum.px()
               << "," << momentum.py()
               << "," << momentum.pz()
               << "," << momentum.e();

    // Restore the stream state
    cout.flags(orig);
    cout.precision(prec);

    GenVertexPtr prod = p->production_vertex();
    GenVertexPtr end  = p->end_vertex();
    int prod_vtx_id   = (prod) ? prod->id() : 0;
    int end_vtx_id    = (end)  ? end->id()  : 0;

    cout << " Stat: " << p->status()
         << " PV: " << prod_vtx_id
         << " EV: " << end_vtx_id
         << endl;
}

void Print::line(shared_ptr<GenCrossSection> &cs) {
    cout << " GenCrossSection: " << cs->cross_section
         << " " << cs->cross_section_error
         << " " << endl;
}

void Print::line(shared_ptr<GenHeavyIon> &hi) {
    cout << " GenHeavyIon: " << hi->Ncoll_hard
         << " " << hi->Npart_proj
         << " " << hi->Npart_targ
         << " " << hi->Ncoll
         << " " << hi->spectator_neutrons
         << " " << hi->spectator_protons
         << " " << hi->N_Nwounded_collisions
         << " " << hi->Nwounded_N_collisions
         << " " << hi->Nwounded_Nwounded_collisions
         << " " << hi->impact_parameter
         << " " << hi->event_plane_angle
         << " " << hi->eccentricity
         << " " << hi->sigma_inel_NN
         << endl;
}

void Print::line(shared_ptr<GenPdfInfo> &pi) {
    cout << " GenPdfInfo: " << pi->parton_id[0]
         << " " << pi->parton_id[1]
         << " " << pi->x[0]
         << " " << pi->x[1]
         << " " << pi->scale
         << " " << pi->xf[0]
         << " " << pi->xf[1]
         << " " << pi->pdf_id[0]
         << " " << pi->pdf_id[1]
         << " " << endl;
}

} // namespace HepMC
