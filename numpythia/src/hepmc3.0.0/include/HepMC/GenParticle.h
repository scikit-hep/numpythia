// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef  HEPMC_GENPARTICLE_H
#define  HEPMC_GENPARTICLE_H
/**
 *  @file GenParticle.h
 *  @brief Definition of \b class GenParticle
 *
 *  @class HepMC::GenParticle
 *  @brief Stores particle-related information
 *
 */
#include "HepMC/Data/SmartPointer.h"
#include "HepMC/Data/GenParticleData.h"
#include "HepMC/FourVector.h"
#include "HepMC/Common.h"

namespace HepMC {


using namespace std;

class GenEvent;
class Attribute;


class GenParticle {

friend class GenEvent;
friend class GenVertex;
friend class SmartPointer<GenParticle>;

//
// Constructors
//
public:
    /** @brief Default constructor */
    GenParticle( const FourVector &momentum = FourVector::ZERO_VECTOR(), int pid = 0, int status = 0 );

    /** @brief Constructor based on particle data */
    GenParticle( const GenParticleData &data );

//
// Functions
//
public:
    /** @brief Check if this particle belongs to an event */
    bool in_event() const { return (bool)(m_event); }

//
// Accessors
//
public:

    GenEvent*              parent_event() const { return m_event; } //!< Get parent event
    int                    id()           const { return m_id;    } //!< Get particle id
    const GenParticleData& data()         const { return m_data;  } //!< Get particle data


    const GenVertexPtr production_vertex() const;        //!< Get production vertex
    const GenVertexPtr end_vertex() const;               //!< Get end vertex

    GenVertexPtr production_vertex();        //!< Get production vertex
    GenVertexPtr end_vertex();               //!< Get end vertex

    /// @brief Convenience access to immediate incoming particles via production vertex
    /// @note Less efficient than via the vertex since return must be by value (in case there is no vertex)
    vector<GenParticlePtr> parents() const;

    /// @brief Convenience access to immediate outgoing particles via end vertex
    /// @note Less efficient than via the vertex since return must be by value (in case there is no vertex)
    vector<GenParticlePtr> children() const;

    /// @brief Convenience access to all incoming particles via production vertex
    vector<GenParticlePtr> ancestors() const;

    /// @brief Convenience access to all outgoing particles via end vertex
    vector<GenParticlePtr> descendants() const;


    int   pid()                   const { return m_data.pid;            } //!< Get PDG ID
    int   status()                const { return m_data.status;         } //!< Get status code
    const FourVector& momentum()  const { return m_data.momentum;       } //!< Get momentum
    bool  is_generated_mass_set() const { return m_data.is_mass_set;    } //!< Check if generated mass is set

    /// @brief Get generated mass
    ///
    /// This function will return mass as set by a generator/tool.
    /// If not set, it will return momentum().m()
    double generated_mass() const;


    void set_pid(int pid);                         //!< Set PDG ID
    void set_status(int status);                   //!< Set status code
    void set_momentum(const FourVector& momentum); //!< Set momentum
    void set_generated_mass(double m);             //!< Set generated mass
    void unset_generated_mass();                   //!< Declare that generated mass is not set


    /** @brief Add an attribute to this particle
     *
     *  This will overwrite existing attribute if an attribute with
     *  the same name is present. The attribute will be stored in the
     *  parent_event(). @return false if there is no parent_event();
     */
    bool add_attribute(string name, shared_ptr<Attribute> att);

    /// @brief Get list of names of attributes assigned to this particle
    vector<string> attribute_names() const;

    /// @brief Remove attribute
    void remove_attribute(string name);

    /// @brief Get attribute of type T
    template<class T>
    shared_ptr<T> attribute(string name) const;

    /// @brief Get attribute of any type as string
    string attribute_as_string(string name) const;


    /// @name Deprecated functionality
    //@{

    #ifndef HEPMC_NO_DEPRECATED

    /// @brief Get PDG ID
    /// @deprecated Use pid() instead
    // HEPMC_DEPRECATED("Use pid() instead")
    int pdg_id() const { return m_data.pid; }

    /// @brief Set PDG ID
    /// @deprecated Use set_pid() instead
    // HEPMC_DEPRECATED("Use set_pid() instead")
    void set_pdg_id(int pidin) { set_pid(pidin); }

    #endif

    //@}
//
// Fields
//
private:
    GenEvent        *m_event; //!< Parent event
    int              m_id;    //!< Index
    GenParticleData  m_data;  //!< Particle data

    weak_ptr<GenVertex>    m_production_vertex; //!< Production vertex
    weak_ptr<GenVertex>    m_end_vertex;        //!< End vertex
    weak_ptr<GenParticle>  m_this;              //!< Pointer to shared pointer managing this particle
};

} // namespace HepMC

#include "HepMC/GenEvent.h"

/// @brief Get attribute of type T
template<class T>
HepMC::shared_ptr<T> HepMC::GenParticle::attribute(string name) const {
  return parent_event()?
    parent_event()->attribute<T>(name, id()): HepMC::shared_ptr<T>();
}

#endif
