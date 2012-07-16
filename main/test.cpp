/**
 * @file test.cpp
 * @brief
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
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author     Anne C. van Rossum
 * @date       Apr 21, 2012
 * @project    Replicator FP7
 * @company    Almende B.V.
 * @case    
 */


#include <stdlib.h>
#include <iostream>
#include <time.h>

#include <ilvq/ILVQ_XSZ.h>
#include <ilvq/defs.h>

#if (RUNONPC==true)
#include <DataDecorator.h>
#include <Plot.h>
#endif

using namespace std;
using namespace dobots;

enum TestCase { TC_CIRCLE, TC_HALVES, TC_COUNT };

TestCase testCase = TC_HALVES;

/**
 * Input: expects x in [0,1], y in [0,1]
 * Will return if
 */
bool withinCircle(float x, float y) {
	return ((2*x-1)*(2*x-1)+(2*y-1)*(2*y-1) < 1.0);
}

bool verticalHalves(float x, float y) {
	return (x < 0.5);
}


void getRandomSample(ILVQ_ASPECT *aspect, ILVQ_CLASS_REPRESENTATION *c) {
	float x = (float)drand48();
	float y = (float)drand48();
	switch (testCase) {
	case TC_CIRCLE:
		*c = withinCircle(x,y) ? 1 : 0;
	break;
	case TC_HALVES:
		*c = verticalHalves(x,y) ? 1 : 0;
	break;
	default:
		*c = verticalHalves(x,y) ? 1 : 0;
		break;
	}
	aspect->clear();
	aspect->push_back(x);
	aspect->push_back(y);
}

int main(int argc, char *argv[]) {
	cout << "Test for ILVQ" << endl;
	srand48( time(NULL) );
	ILVQ_TYPE lambda = 100;
	ILVQ_XSZ *ilvq = new ILVQ_XSZ(lambda);
	ILVQ_ASPECT aspect;
	ILVQ_CLASS_REPRESENTATION class_id;
	int mis_classified = 0, correct_classified = 0;
	int N = 100000;

#if (RUNONPC==true)
	int L = 256;
	float *data = new float[L*L];
	for (int i = 0; i < L; ++i) {
		for (int j = 0; j < L; ++j) {
			data[i+j*L] = 0;
		}
	}
#endif
	for (int t = 0; t < N; ++t) {
		if (t < N*0.9) {
			getRandomSample(&aspect, &class_id);
			ilvq->add(aspect, class_id);
		} else {
			getRandomSample(&aspect, &class_id);
			ILVQ_CLASS_REPRESENTATION cl = ilvq->classify(aspect);
			if (cl == class_id) {
				correct_classified++;
			} else {
				mis_classified++;
			}
#if (RUNONPC==true)
			int x = L * aspect[0];
			int y = L * aspect[1];
			float p = (0.99-(cl*0.5));
			data[x+y*L]=p;
			data[(x+1)%L+y*L]=p;
			data[x+((y+1)%L)*L]=p;
			data[(x+1)%L+((y+1)%L)*L]=p;
#endif
		}
	}
	cout << "Number of prototypes necessary: " << ilvq->getPrototypeCount() << "" << endl;
	cout << "Classified [correct/incorrect]: [" << correct_classified << "/" << mis_classified << "]" << endl;
	delete ilvq;

#if (RUNONPC==true)
	Plot *p = new Plot();
	string f = "ilvq";
	switch (testCase) {
	case TC_CIRCLE:
		f += "_circle";
		break;
	case TC_HALVES:
		f += "_halves";
		break;
	default:
		break;
	}
	p->SetFileName(f, PPM);
	cout << "Write to file: " << f << endl;
	DataContainer &dc = p->GetData();
	dc.SetData(data, L*L);
	p->Draw(PPM);
	delete [] data;
#endif

	return EXIT_SUCCESS;
}


