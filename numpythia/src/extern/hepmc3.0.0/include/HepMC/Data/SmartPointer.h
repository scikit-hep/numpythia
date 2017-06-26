// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/// @brief Definition of \b template \b class SmartPointer

#ifndef  HEPMC_DATA_SMARTPOINTER_H
#define  HEPMC_DATA_SMARTPOINTER_H

#include "HepMC/Common.h"

#if defined(HEPMC_HAS_CXX11) || defined(HEPMC_HAS_CXX0X_GCC_ONLY)

#include <memory>
namespace HepMC {
    using std::weak_ptr;
    using std::shared_ptr;
    using std::make_shared;
    using std::dynamic_pointer_cast;
    using std::const_pointer_cast;
}

#else
#error At least partial C++ 2011 support is required!
#endif


namespace HepMC {


    /// @class HepMC::SmartPointer
    /// @brief Smart pointer for HepMC objects
    ///
    /// Uses shared_ptr to manage the object memory
    ///
    /// @note SmartPointer can be created from raw pointer. This allows
    ///       for implicit conversions when passing raw pointer as argument to
    ///       the constructors or other functions in HepMC classes for backward compatibility.
    ///       SmartPointer ensures only one shared_ptr manages
    ///       the object in such conversions. Note, however, that use of such conversion is deprecated.
    ///
    /// @note Requires managed class to have member field \c weah_ptr<T> \c \b m_this
    ///       used to keep track of shared pointer created to manage the object
    ///
    /// @ingroup data
    ///
    template<class T>
    class SmartPointer {
    public:

        /// @name Constructors
        //@{

        /// Default constructor (NULL pointer)
        SmartPointer();

        /// Copy constructor
        SmartPointer( const SmartPointer<T> &rhs );

        /// Construct SmartPointer using shared pointer
        ///
        /// @note It's advised not to use shared_ptr<T> when using SmartPointer
        /// @note This constructor should be used only in combination with make_shared<T>
        SmartPointer( const shared_ptr<T> &rhs );

        /// Constructor creating shared pointer from raw pointer
        SmartPointer( T *raw_pointer );

        //@}


        /// @name Accessors
        //@{

        /// Assignment
        SmartPointer& operator=(const SmartPointer &rhs) { m_data = rhs.m_data; return *this; }

        /// Equality test
        bool operator==(const SmartPointer &rhs)  const { return  m_data == rhs.m_data; }
        /// Inequality test
        bool operator!=(const SmartPointer &rhs)  const { return  m_data != rhs.m_data; }
        /// Less-than comparison
        bool operator<(const SmartPointer &rhs)  const { return  m_data < rhs.m_data; }

        /// Non-const access to the contained shared_ptr, with non-const contained type
        const shared_ptr<T> operator->() { return  m_data; }
        /// Non-const dereferencing to a reference of the contained type
        T& operator*() { return *m_data; }

        /// Const access to the contained shared_ptr, with const contained type
        /// @note Hurrah for trickery!
        const shared_ptr<const T> operator->() const { return const_pointer_cast<const T>(m_data); }
        /// Const dereferencing to a const reference of the contained type
        const T& operator*() const { return *m_data; }

        /// Bool cast operator
        /// @note This should ideally use the 'safe bool idiom' in C++98 -- in C++11 an implicit explicit
        ///       cast / contextual conversion with the new 'explicit' keyword will be used for safety
        #ifdef HEPMC_HAS_CXX11
        explicit
        #endif
        operator bool() const { return (bool) m_data; }

        //@}


        /// @name Deprecated functions
        //@{

        #ifndef HEPMC_NO_DEPRECATED

        /// Cast to raw pointer
        /// @deprecated Should not be used at all
        HEPMC_DEPRECATED("Use smart pointers instead of raw pointers")
        operator T*() { return m_data.get(); }

        /// Cast to bool
        operator bool() { return (bool) m_data.get(); }

        #endif

        //@}


    private:

        /// @name Fields
        //@{
        /// Shared pointer
        shared_ptr<T> m_data;
        //@}

    };


    /// @name Typedefs for smart pointers to HepMC classes
    //@{
    typedef SmartPointer<class GenParticle> GenParticlePtr; //!< Smart pointer to GenParticle
    typedef SmartPointer<class GenVertex>   GenVertexPtr;   //!< Smart pointer to GenVertex

    typedef SmartPointer<const class GenParticle> ConstGenParticlePtr; //!< Const smart pointer to GenParticle
    typedef SmartPointer<const class GenVertex>   ConstGenVertexPtr;   //!< Const smart pointer to GenVertex

    typedef shared_ptr<class GenPdfInfo>      GenPdfInfoPtr;      //!< Shared pointer to GenPdfInfo
    typedef shared_ptr<class GenHeavyIon>     GenHeavyIonPtr;     //!< Shared pointer to GenHeavyIon
    typedef shared_ptr<class GenCrossSection> GenCrossSectionPtr; //!< Shared pointer to GenCrossSection
    //@}


} // namespace HepMC

#include "HepMC/Data/SmartPointer.icc"

#endif
