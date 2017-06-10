// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
/// @file GenVertex.h
/// @brief Definition of \b class GenVertex
//
#ifndef HEPMC_GENVERTEX_H
#define HEPMC_GENVERTEX_H

#include "HepMC/Data/SmartPointer.h"
#include "HepMC/Data/GenVertexData.h"
#include "HepMC/FourVector.h"
#include "HepMC/Common.h"
#include "HepMC/Errors.h"

namespace HepMC {


    using namespace std;

    // /** @brief Type of iteration. Used by backward-compatibility interface */
    // #ifndef HEPMC_NO_DEPRECATED
    // enum IteratorRange { parents, children, family, ancestors, descendants, relatives };
    // #endif


    class GenEvent;
    class Attribute;


    /// Stores vertex-related information
    class GenVertex {

        /// @todo Are these really needed? Friends usually indicate a problem...
        friend class GenEvent;
        friend class SmartPointer<GenVertex>;


    public:

        /// @name Constructors
        //@{

        /// Default constructor
        GenVertex( const FourVector& position = FourVector::ZERO_VECTOR() );

        /// Constructor based on vertex data
        GenVertex( const GenVertexData& data );

        //@}

    public:

        /// @name Accessors
        //@{

        /// Get parent event
        /// @todo Should we be returning a smart ptr?
        GenEvent* parent_event() const { return m_event; }

        /// Check if this vertex belongs to an event
        /// @todo Needed? Wouldn't it be good enough to just rely on user testing nullness of parent_event()?
        bool in_event() const { return parent_event() != NULL; }

        /// Get the vertex unique identifier
        ///
        /// @note This is not the same as id() in HepMC v2, which is now @c status()
        int id() const { return m_id; }

        /// Get vertex status code
        int status() const { return m_data.status; }
        /// Set vertex status code
        void set_status(int stat) { m_data.status = stat; }

        /// Get vertex data
        const GenVertexData& data() const { return m_data; }

        /// Add incoming particle
        void add_particle_in ( GenParticlePtr p);
        /// Add outgoing particle
        void add_particle_out( GenParticlePtr p);
        /// Remove incoming particle
        void remove_particle_in ( GenParticlePtr p);
        /// Remove outgoing particle
        void remove_particle_out( GenParticlePtr p);

        /// Get list of associated particles
        /// @note Note relatively inefficient return by value
        const vector<GenParticlePtr> particles(Relationship range) const;
        /// Get list of incoming particles
        const vector<GenParticlePtr>& particles_in() const { return m_particles_in; }
        /// Get list of outgoing particles
        const vector<GenParticlePtr>& particles_out() const { return m_particles_out; }

        /// @brief Get vertex position
        ///
        /// Returns the position of this vertex. If a position is not set on _this_ vertex,
        /// the production vertices of ancestors are searched to find the inherited position.
        /// FourVector(0,0,0,0) is returned if no position information is found.
        ///
        /// @todo We need a way to check if there is a position on _this_ vertex, without messing up the interface. Is has_position() too intrusive?
        const FourVector& position() const;
        /// @brief Check if position of this vertex is set
        bool has_set_position() const { return !(m_data.position.is_zero()); }

        /// Set vertex position
        void set_position(const FourVector& new_pos); //!<

        /// @todo We need a way to check if there is a position on _this_ vertex, without messing up the interface. Is has_position() too intrusive?


        /// @brief Add event attribute to this vertex
        ///
        /// This will overwrite existing attribute if an attribute with
        /// the same name is present. The attribute will be stored in the
        /// parent_event(). @return false if there is no parent_event();
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

        /// Get barcode
        ///
        /// @note Currently barcode = id
        // int barcode() const { return m_id; }

        /// Add incoming particle by raw pointer
        /// @deprecated Use GenVertex::add_particle_in( const GenParticlePtr &p ) instead
        HEPMC_DEPRECATED("Use GenParticlePtr instead of GenParticle*")
        void add_particle_in ( GenParticle *p ) { add_particle_in( GenParticlePtr(p) ); }

        /// Add outgoing particle by raw pointer
        /// @deprecated Use GenVertex::add_particle_out( const GenParticlePtr &p ) instead
        HEPMC_DEPRECATED("Use GenParticlePtr instead of GenParticle*")
        void add_particle_out( GenParticle *p ) { add_particle_out( GenParticlePtr(p) ); }

        /// Define iterator by typedef
        typedef vector<GenParticlePtr>::const_iterator particles_in_const_iterator;
        /// Define iterator by typedef
        typedef vector<GenParticlePtr>::const_iterator particles_out_const_iterator;
        /// Define iterator by typedef
        typedef vector<GenParticlePtr>::iterator       particle_iterator;

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Iterate over std container particles_in() instead")
        particles_in_const_iterator  particles_in_const_begin()  const { return m_particles_in.begin();  } //!< @deprecated Backward compatibility iterators

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Iterate over std container particles_in() instead")
        particles_in_const_iterator  particles_in_const_end()    const { return m_particles_in.end();    } //!< @deprecated Backward compatibility iterators

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Iterate over std container particles_out() instead")
        particles_out_const_iterator particles_out_const_begin() const { return m_particles_out.begin(); } //!< @deprecated Backward compatibility iterators

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Iterate over std container particles_out() instead")
        particles_out_const_iterator particles_out_const_end()   const { return m_particles_out.end();   }

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Use particles_in/out() functions instead")
        particle_iterator particles_begin(IteratorRange range) {
            if (range == parents) return m_particles_in.begin();
            if (range == children) return m_particles_out.begin();
            throw Exception("GenVertex::particles_begin: Only 'parents' and 'children' ranges allowed.");
        }

        /// @deprecated Backward compatibility iterators
        HEPMC_DEPRECATED("Use particles_in/out() functions instead")
        particle_iterator particles_end(IteratorRange range) {
            if (range == parents) return m_particles_in.end();
            if (range == children) return m_particles_out.end();
            throw Exception("GenVertex::particles_end: Only 'parents' and 'children' ranges allowed.");
        }

        /// @deprecated Backward compatibility
        HEPMC_DEPRECATED("Use particles_in().size() instead")
        int particles_in_size()  const { return m_particles_in.size(); }

        /// @deprecated Backward compatibility
        HEPMC_DEPRECATED("Use particles_out().size() instead")
        int particles_out_size() const { return m_particles_out.size(); }

        #endif

        //@}


    private:

        /// @name Fields
        //@{
        GenEvent      *m_event;  //!< Parent event
        int            m_id;     //!< Vertex id
        GenVertexData  m_data;   //!< Vertex data

        vector<GenParticlePtr>  m_particles_in;  //!< Incoming particle list
        vector<GenParticlePtr>  m_particles_out; //!< Outgoing particle list
        weak_ptr<GenVertex> m_this;          //!< Pointer to shared pointer managing this vertex
        //@}

    };


} // namespace HepMC

#include "HepMC/GenEvent.h"

/// @brief Get attribute of type T
template<class T>
HepMC::shared_ptr<T> HepMC::GenVertex::attribute(string name) const {
  return parent_event()?
    parent_event()->attribute<T>(name, id()): HepMC::shared_ptr<T>();
}

#endif
