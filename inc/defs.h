/**
 * @brief
 * @file defs.h
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2012 Anne van Rossum <anne@almende.com>
 *
 * @author     Anne C. van Rossum
 * @date       Apr 22, 2012
 * @project    Replicator FP7
 * @company    Almende B.V.
 * @case       Machine learning (fit for Surveyeor robots)
 */


#ifndef DEFS_H_
#define DEFS_H_

#include <vector>
#include <map>

namespace dobots {

typedef float ILVQ_TYPE;

//! One "aspect" is an input vector
typedef std::vector<ILVQ_TYPE> ILVQ_ASPECT;

//! A (multi-modal) vector of multiple(!) aspects
typedef std::vector<ILVQ_ASPECT*> ILVQ_VIEW;

//! A set of views that for example defines an object
typedef std::vector<ILVQ_VIEW*> ILVQ_VIEWS;

//! Normally, the input vectors and the "prototypes" live in the same N-dimensional space in LVQ.
//! The prototype is a "representative" of an object, also often called "subclass"
typedef std::vector<ILVQ_TYPE> ILVQ_PROTOTYPE;

//! The object is represented by multiple "prototypes"
typedef std::vector<ILVQ_PROTOTYPE*> ILVQ_CLASS;

//! We can store multiple classes/objects
typedef std::vector<ILVQ_CLASS*> ILVQ_CLASSES;

//! Prototypes are stored with a key because they can later on be removed as well
//typedef std::map<ILVQ_PROTOTYPE_INDEX, ILVQ_PROTOTYPE*> ILVQ_PROTOTYPES;

//! Index of class
typedef int ILVQ_CLASS_REPRESENTATION;

//! Index of prototype
typedef int ILVQ_PROTOTYPE_INDEX;

//! Representation of class can be distributed
typedef std::vector<ILVQ_TYPE> ILVQ_DISTRIBUTED_CLASS_REPRESENTATION;

//! Mapping from
typedef std::pair<ILVQ_CLASS_REPRESENTATION*,ILVQ_CLASS*> ILVQ_CLASS_MAPPING;

//! Edges between prototypes
typedef std::pair<ILVQ_PROTOTYPE_INDEX,ILVQ_PROTOTYPE_INDEX> ILVQ_PROTOTYPE_CONNECTION;

/**
 * Graph between prototypes
 * Design choice, reading O(1): store outgoing edges for both prototypes, order as vector
 * Go from ILVQ_PROTOTYPE* to ILVQ_PROTOTYPE_INDEX via default "prototypes" array
 */
typedef std::vector<ILVQ_PROTOTYPE*> ILVQ_PROTOTYPE_OUTGOING_CONNECTIONS;
//! Connections are stored per prototype, when prototype has 0 connections, it gets an empty vector assigned to it
typedef std::vector<ILVQ_PROTOTYPE_OUTGOING_CONNECTIONS*> ILVQ_PROTOTYPE_CONNECTIONS;

}


#endif /* DEFS_H_ */
