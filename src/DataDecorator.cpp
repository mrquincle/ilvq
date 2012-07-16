/**
 * @file DataContainer.cpp
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
 * @date	Jul 21, 2011
 * @project	Replicator FP7
 * @company	Almende B.V.
 * @case	Self-organised criticality
 */


// General files
#include <iostream>
#include <locale>
#include <vector>
#include <assert.h>
#include <cmath>
#include <stdio.h>

#include <DataDecorator.h>
#include <EventCounter.hpp>

using namespace std;

/* **************************************************************************************
 * Implementation of DataContainer
 * **************************************************************************************/

DataContainer::DataContainer(): id(-1) {

}

DataContainer::~DataContainer() {

}

/**
 * It is not good to estimate power law distributions by linear regression (see wikipedia,
 * or [1]). Maximum likelihood should be used instead.
 * [1] Power-law Distributions in Empirical Data (2009) Clauset et al.
 */
float DataContainer::CalculateSlope() {
	// alpha estimation = 1 + n [ sum_i^N ln (x_i / (x_min - 1/2) ) ]^-1
	if (dataType != DT_MAP) return -1.0;
	float alpha;
	int x_min = 1; float denom = 1.0 /(x_min - 0.5);

//	int x_max = 10000;

	float sum = 0; int N = 0;
	std::map<DataDecoratorType,int>::iterator it;
	for (it = map_data->begin(); it != map_data->end(); ++it) {
		int value = it->first;
		if (value < x_min) continue;
//		if (value > x_max) continue;
		int count = it->second;
		N += count;
		sum += count * std::log(value * denom);
	}
	cout << "Using x_min=" << x_min << " resulting in n=" << N << " samples" << endl;
	alpha = 1 + N / sum;

	return alpha;
}

/**
 * The number of data elements
 */
int DataContainer::size() {
	switch(dataType) {
	case DT_MAP: assert (map_data != NULL); return map_data->size();
	case DT_F2DARRAY: return float_data_len;
	default:
		cerr << "Size: Unknown data type" << endl;
	}
	return -1;
}

template<>
float DataContainer::item<float>(int index) {
	assert (dataType == DT_F2DARRAY);
	return float_data[index];
}

/**
 * Actually, this function is quite "stupid". A map is not a random access container, so having
 * an index doesn't make sense. A call to item is of order O(N) (and not of order O(1)). However,
 * if it is only used once for plotting or so, everything might be fine.
 */
template<>
pair<DataDecoratorType,int> DataContainer::item< pair<DataDecoratorType,int> >(int index) {
	assert (dataType == DT_MAP);
	std::map<DataDecoratorType,int>::iterator it( map_data->begin() );
	std::advance( it, index );
	return *it;
}

/**
 * Local class that knows that colons can be treated as white spaces. It is
 * used by read.
 */
struct colonsep: std::ctype<char> {
	colonsep(): std::ctype<char>(get_table()) {}

	static std::ctype_base::mask const* get_table() {
		static std::vector<std::ctype_base::mask>
		rc(std::ctype<char>::table_size,std::ctype_base::mask());

		rc[':'] = std::ctype_base::space;
		rc[' '] = std::ctype_base::space;
		rc['\n'] = std::ctype_base::space;
		return &rc[0];
	}
};

/**
 * Read data from a file. Only DT_MAP is tested. With an array it is hard to set the
 * size beforehand properly (except if you know what to retrieve).
 */
void DataContainer::read(std::istream& in) { //, DataDecoratorType resolution) {
	DataDecoratorType x;
	switch(dataType) {
	case DT_MAP:
		assert (map_data != NULL);
		map_data->clear();
		int y;
		in.imbue(std::locale(std::locale(), new colonsep));

		while(in >> x >> y) {
//			if (resolution > 0) {
//				int x_ins = (int)(x * resolution);
//				x = x_ins / (DataDecoratorType)resolution;
//			}
			map_data->insert(make_pair<DataDecoratorType,int>(x, y));
			assert(y != 0);
//			cout << "x and y: " << x << " and " << y << endl;
		}
//		cout << "Read " << map_data->size() << " items" << endl;
		break;
	case DT_F2DARRAY:
		assert (float_data != NULL);
		int ix;
		float fy;
		in.imbue(std::locale(std::locale(), new colonsep));
		while(in >> ix >> fy) {
			if (ix >= float_data_len) {
				cerr << "Array is not large enough (" << float_data_len << ")" << endl;
				break;
			}
			float_data[ix] = fy;
			cout << "x and y: " << ix << " and " << fy << endl;
		}
		break;

	default:
		cerr << "Read: Unknown data type" << endl;
	}
}

/**
 * Write map_data to a file
 */
void DataContainer::write(std::ostream& out) {
	std::map<DataDecoratorType,int>::const_iterator i;
	out << fixed << setprecision (10);
	switch(dataType) {
	case DT_MAP:
		assert (map_data != NULL);
		for (i = map_data->begin(); i != map_data->end(); ++i) {
			out << i->first << ": " << i->second << "\n";
		}
		break;
	case DT_F2DARRAY:
//		cerr << "We don't know how to write this" << endl;
		break;
	default:
		cerr << "Write: Unknown data type" << endl;
	}
}

void DataContainer::clear() {
	switch(dataType) {
	case DT_MAP:
		assert (map_data != NULL);
		map_data->clear();
		break;
	default:
		cerr << "Clear: Unknown data type" << endl;
	}
}

void DataContainer::ApplyBins(int no_bins, DataDecoratorType min, DataDecoratorType max) {
	assert (dataType == DT_MAP);

//	write(std::cout);

	std::map<DataDecoratorType,int>::const_iterator i;
	EventCounter<DataDecoratorType> ec;
	for (i = map_data->begin(); i != map_data->end(); ++i) {
		ec.AddEvent(i->first, i->second);
	}
	ec.Bin(no_bins, min, max);

	map_data->clear();
	for (i = ec.getEvents().begin(); i != ec.getEvents().end(); ++i) {
		map_data->insert(make_pair<DataDecoratorType,int>(i->first, i->second));
	}

//	cout << "After bins: " << endl;
//	write(std::cout);

}
