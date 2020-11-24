
// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2019 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC3_AssociatedParticle_H
#define HEPMC3_AssociatedParticle_H
/**
 *  @file AssociatedParticle.h
 *  @brief Definition of \b class AssociatedParticle,
 *
 *  @class HepMC3::AssociatedParticle
 *  @brief Attribute class allowing
 *  eg. a GenParticle to refer to another GenParticle.

 *  @ingroup attributes
 *
 */

#include "HepMC3/Attribute.h"
#include "HepMC3/GenParticle.h"

namespace HepMC3 {

/**
 *  @class HepMC3::IntAttribute
 *  @brief Attribute that holds an Integer implemented as an int
 *
 *  @ingroup attributes
 */
class AssociatedParticle : public IntAttribute {
public:

    /** @brief Default constructor */
    AssociatedParticle() {}

    /** @brief Constructor initializing attribute value */
    AssociatedParticle(ConstGenParticlePtr p)
        : IntAttribute(p->id()), m_associated(p) {}

    /** @brief Implementation of Attribute::from_string */
    bool from_string(const string &att) {
        IntAttribute::from_string(att);
        if ( associatedId() > int(event()->particles().size()) ||
                associatedId() <= 0  ) return false;
        m_associated = event()->particles()[associatedId() -1];
        return true;
    }

    /** @brief get id of the associated particle. */
    int associatedId() const {
        return value();
    }

    /** @brief get a pointer to the associated particle. */
    ConstGenParticlePtr associated() const {
        return m_associated;
    }

    /** @brief set the value associated to this Attribute. */
    void set_associated(ConstGenParticlePtr p) {
        IntAttribute::set_value(p->id());
        m_associated = p;
    }

private:

    ConstGenParticlePtr m_associated; ///< The associated particle.

};

} // namespace HepMC3

#endif
