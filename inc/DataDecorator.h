/**
 * @file DataContainer.h
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


#ifndef DATADECORATOR_H_
#define DATADECORATOR_H_

// General files
#include <map>

/* **************************************************************************************
 * Interface of DataContainer
 * **************************************************************************************/

enum DataType { DT_MAP, DT_F2DARRAY };

typedef double DataDecoratorType;

/**
 * A "container" class that does not contain data itself, but which can point to different
 * types of data structures. This container is meant to be used in cases where power law
 * behaviour might be observed, so it does have some functionality on-board that can
 * study this type of data.
 */
//template <typename T>
class DataContainer {
public:
	//! Constructor DataContainer
	DataContainer();

	//! Destructor ~DataContainer
	virtual ~DataContainer();

	//! Sets the data type
	inline void SetType(DataType dataType) { this->dataType = dataType; }

	//! Point towards data in the form of a map
	inline void SetData(std::map<DataDecoratorType,int> & data) { this->map_data = &data; dataType = DT_MAP; }

	//! Point towards data in the form of an array
	inline void SetData(float *data, int len) { float_data = data; float_data_len = len; dataType = DT_F2DARRAY; }

	//! Get data item
	template<class T>
	T item(int index);

	//! Return number of data elements
	int size();

	//! Read data from stream (can be a file)
	void read(std::istream& in); //, DataDecoratorType resolution = -1);

	//! Write to file or stream
	void write(std::ostream& out);

	//! Clear the data
	void clear();

	//! Calculate the slope in the loglog plot
	float CalculateSlope();

	//! Id can be used for identification purposes (e.g. in plotting)
	inline void SetID(int id) { this->id = id; }

	//! ID idem
	inline int GetID() { return id; }

	//! Apply bins to the data (in DT_MAP case)
	void ApplyBins(int no_bins, DataDecoratorType min, DataDecoratorType max);
private:
	int id;

	//! Type of data that will be stored
	DataType dataType;

	//! Data in the form of a map
	std::map<DataDecoratorType,int> * map_data;

	//! An array of data
	float *float_data;

	//! Length of float data
	int float_data_len;

};

#endif /* DATADECORATOR_H_ */
