/**
 * @brief Incremental linear vector quantization
 * @file ILVQ.cpp
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

#include <ilvq/ILVQ.h>

#include <functional>
#include <numeric>
#include <iostream>
#include <algorithm>

#include <assert.h>

using namespace dobots;
using namespace std;

/**
 * The norm for a N-dimensional vector, element-wise squaring the difference.
 */
ILVQ_TYPE euclidean(ILVQ_TYPE x, ILVQ_TYPE y) {
	return (x-y)*(x-y);
}

/**
 * Create a template function which moves vector x from or towards y with a learning rate "mu".
 * A positive mu will move "x" away, while a negative mu will move "x" towards "y".
 */
template<typename T>
class op_adjust: std::binary_function<T,T,T> {
	T mu_;
public:
	op_adjust(T mu): mu_(mu) {}
	T operator()(T x, T y) const {
		T result = x+(x-y)*mu_;
//		if (result < 0) result = T(0);
//		if (result > 1) result = T(1);
		return result;
	}
};

ILVQ::ILVQ() {

}

ILVQ::~ILVQ() {

}

/**
 * This function tells something about the "distance" between vectors, in other words the similarity or
 * dissimilarity. There are currently several metrics implemented:
 *   DM_DOTPRODUCT:		return sum_i { x_i*w_i }
 *   DM_EUCLIDEAN:		return sum_i { (x_i-w_i)^2 }
 * It is assumed that the prototype size is equal to the aspect size.
 * @param aspect		in: incoming value
 * @param prototype		in: prototype to check against
 * @param metric		in: a certain distance metric
 * @return				out: the distance between aspect and prototype
 */
ILVQ_TYPE ILVQ::distance(const ILVQ_ASPECT & aspect, const ILVQ_PROTOTYPE & prototype, DistanceMetric metric) {
	if (aspect.size() != prototype.size()) {
		cerr << "Aspect size " << aspect.size() << " while prototype size " << prototype.size() << endl;
		assert (aspect.size() == prototype.size());
	}
	switch (metric) {
	case DM_DOTPRODUCT:
		return inner_product(aspect.begin(), aspect.end(), prototype.begin(), ILVQ_TYPE(0));
	case DM_EUCLIDEAN:
		return inner_product(aspect.begin(), aspect.end(), prototype.begin(), ILVQ_TYPE(0), plus<ILVQ_TYPE>(), euclidean);
	default:
		cerr << "Unknown distance metric" << endl;
		return -1;
	}
}

/**
 * Prototype is adjusted away from the input:
 * w_s = w_s + mu ( x - w_s)
 */
void ILVQ::increaseDistance(ILVQ_PROTOTYPE & prototype, const ILVQ_ASPECT & input, ILVQ_TYPE mu) {
	if (input.size() != prototype.size()) {
		cerr << "Input size " << input.size() << " while prototype size " << prototype.size() << endl;
		assert (input.size() == prototype.size());
	}
	std::transform(prototype.begin(), prototype.end(), input.begin(), prototype.begin(),
			op_adjust<ILVQ_TYPE>(mu));

}

/**
 * Prototype is adjusted away towards the input:
 * w_s = w_s - mu ( x - w_s)
 */
void ILVQ::decreaseDistance(ILVQ_PROTOTYPE & prototype, const ILVQ_ASPECT & input, ILVQ_TYPE mu) {
	if (input.size() != prototype.size()) {
		cerr << "Input size " << input.size() << " while prototype size " << prototype.size() << endl;
		assert (input.size() == prototype.size());
	}
	std::transform(prototype.begin(), prototype.end(), input.begin(), prototype.begin(),
			op_adjust<ILVQ_TYPE>(-mu));
}

void ILVQ::print(ILVQ_ASPECT & vector) {
	cout << "[";
	for (unsigned int i = 0; i < vector.size(); ++i) {
		cout << vector[i] << " ";
	}
	cout << "]";
}
