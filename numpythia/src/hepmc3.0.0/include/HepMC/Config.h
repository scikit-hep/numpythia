// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC_CONFIG_H
#define HEPMC_CONFIG_H


// The Reader/Writer class separation is available
#ifndef HEPMC_HAS_SPLIT_IO
#define HEPMC_HAS_SPLIT_IO 1
#endif

// Units are defined in HepMC -- version 2 of the units system is simpler
#ifndef HEPMC_HAS_UNITS
#define HEPMC_HAS_UNITS 2
#endif

// The GenPdfInfo class is available
#ifndef HEPMC_HAS_PDF_INFO
#define HEPMC_HAS_PDF_INFO 2
#endif

// The GenCrossSection class is available
#ifndef HEPMC_HAS_CROSS_SECTION
#define HEPMC_HAS_CROSS_SECTION 2
#endif

// The GenHeavyIon class is available
#ifndef HEPMC_HAS_HEAVY_ION
#define HEPMC_HAS_HEAVY_ION 2
#endif

// The HepMC::GenWeights class allows named weights (version 2)
#ifndef HEPMC_HAS_NAMED_WEIGHTS
#define HEPMC_HAS_NAMED_WEIGHTS 2
#endif

// The HepMC::GenVertex class has a status() member
#ifndef HEPMC_VERTEX_HAS_STATUS
#define HEPMC_VERTEX_HAS_STATUS 1
#endif

// The HepMC::HeavyIon class contains centrality
#ifndef HEPMC_HEAVY_ION_HAS_CENTRALITY
#define HEPMC_HEAVY_ION_HAS_CENTRALITY 1
#endif

// Particles and vertices iterators have appropriate constness in method declarations and return types
#ifndef HEPMC_HAS_CONSISTENT_CONST
#define HEPMC_HAS_CONSISTENT_CONST 2
#endif

// Particles and vertices can be retrieved as vectors of pointers as well as the verbose iterators
#ifndef HEPMC_HAS_SIMPLE_RANGES
#define HEPMC_HAS_SIMPLE_RANGES 1
#endif


/// DEPRECATED STUFF:

// The IO_GenEvent class is available (for now)
#ifndef HEPMC_HAS_IO_GENEVENT
#define HEPMC_HAS_IO_GENEVENT 1
#endif

// The iterator range classes are available
#ifndef HEPMC_HAS_ITERATOR_RANGES
#define HEPMC_HAS_ITERATOR_RANGES 1
#endif


#endif
