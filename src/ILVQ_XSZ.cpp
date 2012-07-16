/**
 * @brief
 * @file ILVQ_XSZ.cpp
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

#include <ilvq/ILVQ_XSZ.h>

#include <map>
#include <algorithm>
#include <limits>
#include <numeric>
#include <iostream>
#include <assert.h>

#include <sys/syslog.h>

using namespace dobots;
using namespace std;

ILVQ_XSZ::ILVQ_XSZ(int ageOld, ILVQ_TYPE mu1, ILVQ_TYPE mu2, int lambda): ageOld(ageOld),
		mu1(mu1),
		mu2(mu2),
		lambda(lambda),
		lambda_i(0) {
	debug = LOG_ERR;
}

ILVQ_XSZ::~ILVQ_XSZ() {

}

void ILVQ_XSZ::add(ILVQ_ASPECT & input, ILVQ_CLASS_REPRESENTATION & class_rep) {
	if (debug >= LOG_INFO) {
		cout << __func__ << ": input=";
		print(input);
		cout << ", class=" << class_rep << endl;
	}
	getClosePrototypes(input, temp_winners);
	if (isNewPrototype(input, class_rep, temp_winners)) {
		ILVQ_XSZ_PROTOTYPE *p = new ILVQ_XSZ_PROTOTYPE();
		p->T_s = 0;
		p->class_id = class_rep;
		p->outgoing_connections = new ILVQ_XSZ_CONNECTIONS();
		p->prototype = new ILVQ_ASPECT(input);
		//		*p->prototype = input;
		assert (p->prototype->size() == input.size());
		p->winner_count = 0;
		prototypes.insert(p);
		updateThreshold(*p);
	} else
		// additional check for emptiness, but should be only the first two times
		if (temp_winners.s1 && temp_winners.s2) {
			addEdge(temp_winners.s1, temp_winners.s2);
			updateLearningRates(*temp_winners.s1);
			updatePrototype(*temp_winners.s1, input, class_rep);
			updateThreshold(*temp_winners.s1);
			deleteEdges();
		}
	temp_winners.s1 = NULL;
	temp_winners.s2 = NULL;
	if (lambda == lambda_i) {
		deleteNodes();
		lambda_i = 0;
	}
	lambda_i++;
}

int ILVQ_XSZ::getPrototypeCount() {
	return prototypes.size();
}

ILVQ_CLASS_REPRESENTATION ILVQ_XSZ::classify(ILVQ_ASPECT & input) {
	getClosePrototypes(input, temp_winners);
	return temp_winners.s1->class_id;
}

/**
 * Returns the two closest prototypes to the given input.
 */
void ILVQ_XSZ::getClosePrototypes(const ILVQ_ASPECT & input, ILVQ_XSZ_PROTOTYPE_PAIR & winners) {
	std::set<ILVQ_XSZ_PROTOTYPE*>::const_iterator it;
	ILVQ_TYPE winner_value = numeric_limits<ILVQ_TYPE>::max();
	ILVQ_TYPE runnerup_value = numeric_limits<ILVQ_TYPE>::max();
	winners.s1 = winners.s2 = NULL;
	if (debug >= LOG_DEBUG) {
		cout << "Number of prototypes: " << prototypes.size() << endl;
	}
	int index = 0;
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		ILVQ_XSZ_PROTOTYPE *p = *it;
		ILVQ_TYPE dist = distance(input, *p->prototype, DM_EUCLIDEAN);
		if (debug >= LOG_ERR) index++;
		if (dist < winner_value) {
			winners.s2 = winners.s1;
			runnerup_value = winner_value;
			winners.s1 = p;
			winner_value = dist;
			if (debug >= LOG_DEBUG) {
				cout << "Distance to prototype " << index << " becomes: ";
				print(*p->prototype);
				cout << "=" << dist;
				cout << " and has class id " << winners.s1->class_id << endl;
			}
		} else if (dist < runnerup_value) {
			winners.s2 = p;
			runnerup_value = dist;
		}
	}
	if (debug >= LOG_INFO) {
		if (winners.s1 != NULL) {
			cout << "Winner is: ";
			print(*winners.s1->prototype);
			cout << " with distance=" << winner_value;
			cout << " and class id " << winners.s1->class_id << endl;
		}
		if (winners.s2 != NULL) {
			cout << "Runner-up is: ";
			print(*winners.s2->prototype);
			cout << " with distance=" << runnerup_value;
			cout << " and class id " << winners.s2->class_id << endl;
		}
	}
}

bool ILVQ_XSZ::isNewPrototype(const ILVQ_ASPECT & input, ILVQ_CLASS_REPRESENTATION & class_rep,
		const ILVQ_XSZ_PROTOTYPE_PAIR & winners) {
	if (!winners.s1 || !winners.s2) {
		if (debug >= LOG_DEBUG)
			cout << "No two winners available" << endl;
		return true;
	}
	ILVQ_TYPE dT1 = distance(input, *winners.s1->prototype, DM_EUCLIDEAN);
	if (dT1 > winners.s1->T_s) {
		if (debug >= LOG_DEBUG)
			cout << "Far enough from winner: " << dT1 << " > " << winners.s1->T_s << endl;
		return true;
	}
	ILVQ_TYPE dT2 = distance(input, *winners.s2->prototype, DM_EUCLIDEAN);
	if (dT2 > winners.s2->T_s) return true;
	if (isNewClass(class_rep)) return true;
	if (debug >= LOG_INFO) {
		cout << "Prototype is not new, we will adjust the weights" << endl;
	}
	return false;
}

bool ILVQ_XSZ::isNewClass(ILVQ_CLASS_REPRESENTATION & class_rep) {
	std::set<ILVQ_XSZ_PROTOTYPE*>::const_iterator it;
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		if ((*it)->class_id == class_rep) return false;
	}
	return true;
}

/**
 * Creation of an edge if it does not exist. Updating the ages of the outgoing edges.
 * And updating the winning count of the source node.
 */
void ILVQ_XSZ::addEdge(ILVQ_XSZ_PROTOTYPE *s1, ILVQ_XSZ_PROTOTYPE *s2) {
	// update edge of outgoing edges
	//	cout << "Increment age" << endl;
	ILVQ_XSZ_CONNECTIONS *e = s1->outgoing_connections;
	bool exist = false;
	if (e != NULL) {
		ILVQ_XSZ_CONNECTIONS::const_iterator it_e;
		for (it_e = e->begin(); it_e != e->end(); ++it_e) {
			if ((*it_e)->s2 == s2) {
				exist = true;
			}
			(*it_e)->age++;
		}
	}
	// add the edge if it doesn't exist
	if (!exist) {
		ILVQ_XSZ_CONNECTION *c = new ILVQ_XSZ_CONNECTION();
		c->s1 = s1;
		c->s2 = s2;
		c->age = 0;
		s1->outgoing_connections->push_back(c);
		if (debug >= LOG_DEBUG) {
			cout << __func__ << ": Add edge between ";
			print(*c->s1->prototype);
			cout << " and ";
			print(*c->s2->prototype);
			cout << endl;
		}
	}
	// update winner count
	s1->winner_count++;
	//	cout << "Increment winner count" << endl;
}

/**
 * Updating the prototypes towards or from the input.
 */
void ILVQ_XSZ::updatePrototype(ILVQ_XSZ_PROTOTYPE &winner, const ILVQ_ASPECT & input,
		ILVQ_CLASS_REPRESENTATION & class_rep) {
	if (winner.class_id == class_rep) {
//		ILVQ_TYPE dist_pre = distance(*winner.prototype, input, DM_EUCLIDEAN);
		decreaseDistance(*winner.prototype, input, mu1);
//		ILVQ_TYPE dist_post = distance(*winner.prototype, input, DM_EUCLIDEAN);
//		cout << "Distance increased with " << dist_post - dist_pre << endl;
		ILVQ_XSZ_CONNECTIONS &e = *winner.outgoing_connections;
		ILVQ_XSZ_CONNECTIONS::const_iterator it_e;
		for (it_e = e.begin(); it_e != e.end(); ++it_e) {
			increaseDistance(*(*it_e)->s2->prototype, input, mu2);
		}
	} else {
		increaseDistance(*winner.prototype, input, mu1);
		ILVQ_XSZ_CONNECTIONS &e = *winner.outgoing_connections;
		ILVQ_XSZ_CONNECTIONS::const_iterator it_e;
		for (it_e = e.begin(); it_e != e.end(); ++it_e) {
			decreaseDistance(*(*it_e)->s2->prototype, input, mu2);
		}
	}
}

/**
 * Somewhere the authors describe an adaptive mechanism to adjust the learning rate. It is not
 * entirely clear when to set this. We choose here to set it BEFORE we adjust the winner and
 * hence the winner count does influence how fast the adaptation goes. This means that the
 * learning rate hops and jumps every time we select another winner...
 */
void ILVQ_XSZ::updateLearningRates(ILVQ_XSZ_PROTOTYPE &winner) {
	mu1 = 0.5;
	if (winner.winner_count > 10) {
		mu1 = mu1 / (winner.winner_count / 10);
//		cout << "Now mu becomes " << mu1 << endl;
	}
	mu2 = mu1 / 100.0;
}

bool comp_dist(std::pair<ILVQ_TYPE,ILVQ_XSZ_CONNECTION*> p0, std::pair<ILVQ_TYPE,ILVQ_XSZ_CONNECTION*> p1) {
	return (p0.first < p1.first);
}

/**
 * Update the threshold T_winner.
 */
void ILVQ_XSZ::updateThreshold(ILVQ_XSZ_PROTOTYPE &winner) {
	ILVQ_CLASS_REPRESENTATION class_id = winner.class_id;

	// first calculate the "within class" threshold
	ILVQ_TYPE T_within = ILVQ_TYPE(0);
	int within_members = 0;
	std::set<ILVQ_XSZ_PROTOTYPE*>::const_iterator it;
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		if ((*it)->class_id == class_id) {
			ILVQ_XSZ_CONNECTIONS *e = (*it)->outgoing_connections;
			ILVQ_XSZ_CONNECTIONS::const_iterator it_e;
			for (it_e = e->begin(); it_e != e->end(); ++it_e, ++within_members) {
				T_within += distance(*(*it_e)->s1->prototype, *(*it_e)->s2->prototype, DM_EUCLIDEAN);
			}
		}
	}
	T_within = T_within / within_members;
	if (debug >= LOG_DEBUG)
		cout << __func__ << ": the average within class distance is calculated as " << T_within << endl;

	// then calculate "between class"
	std::vector<std::pair<ILVQ_TYPE,ILVQ_XSZ_CONNECTION*> > conn;
	ILVQ_TYPE T_dist = ILVQ_TYPE(0);
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		ILVQ_XSZ_CONNECTIONS *e = (*it)->outgoing_connections;
		assert (e);
		ILVQ_XSZ_CONNECTIONS::const_iterator it_e;
		for (it_e = e->begin(); it_e != e->end(); ++it_e) {
			assert ((*it_e)->s1);
			assert ((*it_e)->s2);
			assert ((*it_e)->s1->prototype != NULL);
			assert ((*it_e)->s2->prototype != NULL);
			if ((*it_e)->s2->class_id == class_id) {
				T_dist = distance(*(*it_e)->s1->prototype, *(*it_e)->s2->prototype, DM_EUCLIDEAN);
				conn.push_back(make_pair<ILVQ_TYPE,ILVQ_XSZ_CONNECTION*>(T_dist,*it_e));
			}
		}
	}

	// sort on distance
	std::sort(conn.begin(), conn.end(), comp_dist);
	if (debug >= LOG_DEBUG) {
		cout << __func__ << ": sorted distances {";
		for (unsigned int i = 0; i < conn.size(); ++i) {
			cout << conn[i].first << " ";
		}
		cout << "}" << endl;
	}

	// loop through distances till the one between is indeed larger than the (averaged) within class distance
	if (conn.size() > 1) {
		ILVQ_TYPE T_between = conn[0].first;
		winner.T_s = T_between;
		for (unsigned int i = 1; i < conn.size(); ++i) {
			winner.T_s = T_between;
			T_between = conn[i].first;
			if (T_between > T_within)
				break;
		}
	} else {
		winner.T_s = T_within;
	}

	if (debug >= LOG_DEBUG) {
		cout << __func__ << ": the new threshold for the winner becomes: " << winner.T_s << endl;
	}
	// remove the pairs again
	conn.erase(conn.begin(), conn.end());
}

/**
 * Helper class to delete old edges
 */
class delete_old {
	int old;
public:
	delete_old(int age): old(age) {}
	bool operator()(ILVQ_XSZ_CONNECTION *c) const {
		return (c->age >= old);
	}
};

/**
 * Delete edges that are too old.
 */
void ILVQ_XSZ::deleteEdges() {
	std::set<ILVQ_XSZ_PROTOTYPE*>::const_iterator it;
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		ILVQ_XSZ_CONNECTIONS &e = *(*it)->outgoing_connections;
		e.erase(std::remove_if(e.begin(), e.end(), delete_old(ageOld)), e.end());
	}
}

class delete_target {
	ILVQ_XSZ_PROTOTYPE* target_;
public:
	delete_target(ILVQ_XSZ_PROTOTYPE* target): target_(target) {}
	bool operator()(ILVQ_XSZ_CONNECTION *c) const {
		return (c->s2 == target_);
	}
};

void ILVQ_XSZ::deleteEdges(ILVQ_XSZ_PROTOTYPE* target) {
	std::set<ILVQ_XSZ_PROTOTYPE*>::const_iterator it;
	for (it = prototypes.begin(); it != prototypes.end(); ++it) {
		ILVQ_XSZ_CONNECTIONS &e = *(*it)->outgoing_connections;
		e.erase(std::remove_if(e.begin(), e.end(), delete_target(target)), e.end());
	}
}

//! Delete node on two conditions
struct delete_empty_node {
	bool operator()(ILVQ_XSZ_PROTOTYPE *p) const {
		return (p->outgoing_connections->empty());
	}
};

class delete_lonely_node {
	ILVQ_TYPE M_;
public:
	delete_lonely_node(ILVQ_TYPE M): M_(M) {}
	bool operator()(ILVQ_XSZ_PROTOTYPE *p) const {
		return ((p->outgoing_connections->size() == 1) && (p->winner_count < M_));
	}
};

int sum_winner_count(int sum, ILVQ_XSZ_PROTOTYPE *p) {
	return sum + p->winner_count;
}

/**
 * Prototypes is a set, so we have to "while through it" and cannot use the erase(remove_if)
 * template.
 */
void ILVQ_XSZ::deleteNodes() {
	std::set<ILVQ_XSZ_PROTOTYPE*>::iterator cur, tmp;
	cur = prototypes.begin();
	const std::set<ILVQ_XSZ_PROTOTYPE*>::iterator last = prototypes.end();
	while ((cur = std::find_if(cur, last, delete_empty_node())) != last) {
		tmp = cur++;
		assert ((*tmp)->prototype != NULL);
		// should not have edges going in either, but who cares, to be sure:
		deleteEdges(*tmp);
		if (debug >= LOG_DEBUG) {
			cout << "Delete prototype without connections ";
			print(*(*tmp)->prototype);
			cout << endl;
		}
		delete (*tmp)->prototype;
		prototypes.erase(tmp);
	}
	cur = prototypes.begin();
	ILVQ_TYPE M = std::accumulate(prototypes.begin(), prototypes.end(), 0, sum_winner_count);
	M /= (prototypes.size()*2.0);
	while ((cur = std::find_if(cur, last, delete_lonely_node(M))) != last) {
		tmp = cur++;
		assert ((*tmp)->prototype != NULL);
		// delete incoming edges
		deleteEdges(*tmp);
		// delete outgoing edges
		(*tmp)->outgoing_connections->erase((*tmp)->outgoing_connections->begin(), (*tmp)->outgoing_connections->end());
		if (debug >= LOG_DEBUG) {
			cout << "Delete prototype with single connection ";
			print(*(*tmp)->prototype);
			cout << endl;
		}
		delete (*tmp)->prototype;
		prototypes.erase(tmp);
	}
}
