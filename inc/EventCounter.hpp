/**
 * @file EventCounter.h
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
 * @author 	Anne C. van Rossum
 * @date	Jul 18, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */


#ifndef EVENTCOUNTER_H_
#define EVENTCOUNTER_H_

// General files
#include <map>
#include <iostream>
#include <iomanip>
#include <iterator>

/* **************************************************************************************
 * Interface of EventCounter
 * **************************************************************************************/

/**
 * Count events of a given "type" (might e.g. be size)
 */
template <typename T>
class EventCounter {
public:
	//! Construct an event counter
	EventCounter() { events.clear(); }

	//! Add one event of given type
	void AddEvent(const T type) {
		typename std::map<T,int>::iterator f = events.find(type);
		if (f == events.end()) {
			events.insert(std::make_pair<T,int>(type, 1));
		} else {
			(*f).second++;
		}
	}

	//! Add "freq" events of a given type
	void AddEvent(const T type, int freq) {
		typename std::map<T,int>::iterator f = events.find(type);
		if (f == events.end()) {
			events.insert(std::make_pair<T,int>(type, freq));
		} else {
			(*f).second += freq;
		}
	}

	//! Take the existing events and put them in bins
	void Bin(int no_bins, T min, T max) {
		typename std::map<T,int>::iterator f;
		if (events.empty()) return;
		EventCounter binned_cntr;

		T delta = (max - min) / no_bins;
		std::cout << "Delta is " << delta << std::endl;
		for (f = events.begin(); f != events.end(); ++f) {
			T value = (*f).first;
			int bin_id = 0;
			if (value >= min) bin_id = (value - min) / delta;
			if (value > max) bin_id = no_bins;
			T bin_value = min + delta * bin_id;
//			std::cout << "Bin id is " << bin_id << "(" << value << "-" << min << ") /" << no_bins << " and bin value thus: " << bin_value << std::endl;
			binned_cntr.AddEvent(bin_value, (*f).second);
		}
		int size = events.size();
		events.clear();
		for (f = binned_cntr.getEvents().begin(); f != binned_cntr.getEvents().end(); ++f) {
			AddEvent(f->first, f->second);
		}
		std::cerr << "Goes from size " << size << " to " << events.size() << std::endl;
		if (events.size() < 10) {
			std::cerr << "Maybe use more than " << no_bins << " bins" << std::endl;
		}

		// add e.g. assert on nr of events (should be the same)
	}

	//! Print
	void Print(int print_list = 0) {
		typename std::map<T,int>::iterator f;

		int line_items = 20; int i = 0;

		switch (print_list) {
		case 0:
			for (f = events.begin(); f != events.end(); ++f) {
				std::cout << (*f).first << ":" << (*f).second << " ";
				if (++i == line_items) { std::cout << std::endl; i = 0; }
			}
			break;
		case 1:
			++i;
			for (f = events.begin(); f != events.end(); ++f, ++i) {
				int index = (*f).first;
				for (int j = 0; j < index - i; ++i) {
					std::cout << std::setw(2) << 0 << " ";
					if (!(i % line_items)) std::cout << std::endl;
				}
				std::cout << std::setw(2) << (*f).second << " ";
				if (!(i % line_items)) std::cout << std::endl;
			}
			break;
		default:
			std::cerr << "Unknown print mode" << std::endl;
		}
		std::cout << std::endl;

	}

	//! Get events
	std::map<T, int> & getEvents() { return events; }
private:
	//! Probably a sparse vector is the best, but to stay in STL, we will use a map
	std::map<T, int> events;
};

#endif /* EVENTCOUNTER_H_ */
