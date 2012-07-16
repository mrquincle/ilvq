/**
 * @brief Incremental linear vector quantization
 * @file ILVQ.h
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


#ifndef ILVQ_H_
#define ILVQ_H_

#include <ilvq/defs.h>

namespace dobots {

enum DistanceMetric { DM_EUCLIDEAN, DM_DOTPRODUCT, DM_TYPES };

class ILVQ {
public:
	ILVQ();

	virtual ~ILVQ();

	//! Add new input
	virtual void add(ILVQ_ASPECT & input, ILVQ_CLASS_REPRESENTATION & class_rep) = 0;

	//! Calculate the distance between aspect and prototype
	ILVQ_TYPE distance(const ILVQ_ASPECT & aspect, const ILVQ_PROTOTYPE & prototype, DistanceMetric metric);

	//! Increase the distance given a new input (by updating prototype)
	void increaseDistance(ILVQ_PROTOTYPE & prototype, const ILVQ_ASPECT & input, ILVQ_TYPE mu);

	//! Decrease distance (by updating prototype)
	void decreaseDistance(ILVQ_PROTOTYPE & prototype, const ILVQ_ASPECT & input, ILVQ_TYPE mu);

protected:
	//! For debugging purposes
	void print(ILVQ_ASPECT & vector);
private:
};

}

#endif /* ILVQ_H_ */
