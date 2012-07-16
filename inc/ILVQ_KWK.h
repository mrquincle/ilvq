/**
 * @brief
 * @file ILVQ_KWK.h
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


#ifndef ILVQ_KWK_H_
#define ILVQ_KWK_H_

#include <ilvq/defs.h>
#include <ilvq/ILVQ.h>

namespace dobots {

/*
 * First, I picked this one: "Rapid Online Learning of Objects in a Biologically Motivated
 * Recognition Architecture" by Kirstein, Wersing, Körner (2005). However, it is vague at many
 * point so I decided to switch to "An incremental learning vector quantization algorithm for
 * pattern classification" by Xu, Shen, Zhao (2010).
 */
class ILVQ_KWK: public ILVQ {
public:

	/**
	 * Similarity is measured as: A_i^l = exp ( -||x^i - r^l||^2 / sigma )
	 */
	void similarity(ILVQ_ASPECT aspect, ILVQ_PROTOTYPE prototype);

	/**
	 * Iterate through all prototypes of a class and returns maximum similarity:
	 *   A_i^max = max { A_i^l }
	 */
	float max_similar(ILVQ_ASPECT aspect, ILVQ_CLASS_INDEX class_index);

	/**
	 * If too similar, do not accept the new aspect. Too similar is just defined
	 * simple by:
	 *   A_i^max >= S_t.
	 * This function hence returns "true" A_i^max < S_t.
	 */
	bool accept(float similarity);

private:
	// Similarity threshold, if exceeded a new aspect will not be incorporated
	float S_t;

	//! Chosen such that average similarity is appr. equal to 0.5
	float sigma;

};

}

#endif /* ILVQ_KWK_H_ */
