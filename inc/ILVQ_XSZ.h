/**
 * @brief
 * @file ILVQ_XSZ.h
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


#ifndef ILVQ_XSZ_H_
#define ILVQ_XSZ_H_

#include <ilvq/defs.h>
#include <ilvq/ILVQ.h>

#include <map>
#include <set>
#include <list>
#include <algorithm>

namespace dobots {

struct ILVQ_XSZ_CONNECTION;

typedef std::list<ILVQ_XSZ_CONNECTION*> ILVQ_XSZ_CONNECTIONS;

struct ILVQ_XSZ_PROTOTYPE {
	ILVQ_PROTOTYPE *prototype;
	ILVQ_TYPE T_s; // state
	int winner_count; //M_s
	ILVQ_CLASS_REPRESENTATION class_id; // class represented by index/id (not distributed)
	ILVQ_XSZ_CONNECTIONS *outgoing_connections;
};

struct ILVQ_XSZ_CONNECTION {
	ILVQ_XSZ_PROTOTYPE *s1;
	ILVQ_XSZ_PROTOTYPE *s2;
	int age;
};

typedef ILVQ_XSZ_CONNECTION ILVQ_XSZ_PROTOTYPE_PAIR;

/**
 * First, I picked this one: "Rapid Online Learning of Objects in a Biologically Motivated
 * Recognition Architecture" by Kirstein, Wersing, Körner (2005). However, it is vague at many
 * point so I decided to switch to "An incremental learning vector quantization algorithm for
 * pattern classification" by Xu, Shen, Zhao (2010).
 * iLVQ is an incremental approach, so not all training examples need to be there. This means it
 * fits the online learning paradigm useful for autonomous robotics.
 * In Xu et al.'s paper a winner and a runner-up are used in the comparison. The difference in
 * activation of either one indicates how much an input vector as a borderline case. And for
 * classification purposes the authors argue that borderline cases are more effective than
 * central prototypes. Personally, I think this depends on then "nature" of the border however
 * (and over-representation of non-smooth borders while in practice they can be smooth and
 * represented by a few more central prototypes). Anyway, it is more important to remove
 * outliers if so actively searched for borderline prototypes because they can be erroneously
 * classified or sensor noise for example. Hence, Xu et al. also introduce a condensing scheme
 * for denoising.
 */
class ILVQ_XSZ: public ILVQ {
public:
	ILVQ_XSZ(int ageOld=16, ILVQ_TYPE mu1=0.1, ILVQ_TYPE mu2=0.001, int lambda=16);

	~ILVQ_XSZ();

	void add(ILVQ_ASPECT &input, ILVQ_CLASS_REPRESENTATION & class_rep);

	ILVQ_CLASS_REPRESENTATION classify(ILVQ_ASPECT & input);

	int getPrototypeCount();

protected: // everything that is protected can use ILVQ_XSZ_PROTOTYPE instead of ILVQ_PROTOTYPE
	/**
	 * Obtain the winner and runner-up given a new input vector.
	 */
	void getClosePrototypes(const ILVQ_ASPECT & input, ILVQ_XSZ_PROTOTYPE_PAIR & winners);

	/**
	 * Calculate
	 */
	bool isNewPrototype(const ILVQ_ASPECT & input, ILVQ_CLASS_REPRESENTATION & class_rep,
			const ILVQ_XSZ_PROTOTYPE_PAIR & winners);

	//! Slow searching for class id through prototype set
	bool isNewClass(ILVQ_CLASS_REPRESENTATION & class_rep);

	//! Add edge (plus update ages and winner count)
	void addEdge(ILVQ_XSZ_PROTOTYPE *s1, ILVQ_XSZ_PROTOTYPE *s2);

	//! Move prototype toward or from input
	void updatePrototype(ILVQ_XSZ_PROTOTYPE &winner, const ILVQ_ASPECT & input,
			ILVQ_CLASS_REPRESENTATION & class_rep);

	//! Dynamic update of learning rates to most recent winner
	//! I guess it makes only sense to call before updatePrototype
	void updateLearningRates(ILVQ_XSZ_PROTOTYPE &winner);

	void updateThreshold(ILVQ_XSZ_PROTOTYPE &winner);

	/**
	 * Delete all edges with age > AgeOld.
	 */
	void deleteEdges();

	//! Delete the unconnected and sparse connected
	void deleteNodes();

protected:
	//! Delete edges leading to given node (used by deleteNodes)
	void deleteEdges(ILVQ_XSZ_PROTOTYPE* target);
private:
	//! Global variable that removes old edges
	int ageOld;

	//! Learning rates
	ILVQ_TYPE mu1, mu2;

	//! Clean up parameter
	int lambda;

	//! Iterate step index
	int lambda_i;

	//! Debug setting
	char debug;

	//! Contains all prototypes (G)
	std::set<ILVQ_XSZ_PROTOTYPE*> prototypes;

	//! Temporary field, not meant to be accessed directly, just memory allocations
	ILVQ_XSZ_PROTOTYPE_PAIR temp_winners;
};

}

#endif /* ILVQ_XSZ_H_ */
