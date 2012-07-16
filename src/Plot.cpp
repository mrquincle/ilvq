/**
 * @file Plot.cpp
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


// General files
#include <Plot.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <vector>

#include <boost/lexical_cast.hpp>

#define VERBOSE

using namespace std;

/* **************************************************************************************
 * Implementation of Plot
 * **************************************************************************************/

/**
 * Create a new OpenPL stream.
 */
Plot::Plot() {
	//	data = new DataContainer();
	current_container = -1;

	svg_file = "plot"; // .svg
	ppm_file = "image";  //.ppm

	pls = NULL;
	path = "";

	plot_mode = PM_DEFAULT;
	plot_type = PT_DEFAULT;

	dimensions_set = false;
}

//! Get the data
DataContainer & Plot::GetData(int id) {
	if (id == -1) {
		if (current_container == -1) {
			DataContainer *dc = new DataContainer();
			data_v.push_back(dc);
			id++;
		} else {
			if (current_container >= (int)data_v.size()) {
				cerr << "Error! Current container index (" << current_container
						<< ") should be smaller than data size (" << data_v.size() << ")" << endl;
			}
			return *data_v[current_container];
		}
	}

	while (id >= (int)data_v.size()) {
		DataContainer *dc = new DataContainer();
		data_v.push_back(dc);
	}
	current_container = id;
	if (current_container >= (int)data_v.size()) {
		cerr << "Error! Container index (" << current_container
				<< ") should be smaller than data size (" << data_v.size() << ")" << endl;
	}
	return *data_v[current_container];
}

/**
 * Initialisation is separate, so we can use SetFileName etc. beforehand...
 */
void Plot::Init(OutputType outputType) {
	if (outputType == PLPLOT) {
		pls = new plstream();
		string svg_name = path + svg_file + ".svg";
		pls->sfnam(svg_name.c_str());
		pls->sdev("svg");

		pls->scolbg(255,255,255);

		//	pls->legend?
		// Initialize plplot
		pls->init();

		//    pls->sesc( '@' );
		pls->font( 2 );

//		type = 0;
		x_label = "x-axis ()";
		y_label = "y-axis ()";
		alty_label = "y-axis right ()";
		title_label = "Title of plot";
		line_label = "Line note...";
	}
	// do not need to initialise if output type is PPM
}

/**
 * Delete plot object.
 */
Plot::~Plot() {
	current_container = -1;
	//	if (data != NULL) delete data;
	if (pls != NULL) delete pls;
}

/**
 * Draws the map_data, can only be done after SetData has been called. The map_data
 * should be in a map<int,int> format where the first value of each pair is
 * the x-coordinate, and the second the y-coordinate.
 * @param data2file		Store data in a file
 * @param file2data		Read data from a file
 */
void Plot::Draw(OutputType outputType) { //, bool data2file, bool file2data) {
	switch (outputType) {
	case PLPLOT: DrawPLPlot(); break; //data2file, file2data); break;
	case PPM: DrawPPM(); break;
	default:
		cerr << "Unknown output type" << endl;
	}
}

/**
 * Set the filename. It will be prepended by the path that is set.
 */
void Plot::SetFileName(std::string filename, OutputType outputType) {
	switch (outputType) {
	case PLPLOT: svg_file = filename; break;
	case PPM: ppm_file = filename; break;
	}
}

/**
 * Set path that will be used to store the pictures / plots into.
 */
void Plot::SetPath(std::string path) {
	this->path = path;
}

/**
 * Scale output depending on the plot mode
 */
PLFLT Plot::Scale(const PLFLT input, bool x_axis) {
	switch(plot_mode) {
	case PM_SEMILOG:
		if (!x_axis) {
			assert (input > 0);
			return log10(input);
		}
		return input;
	case PM_DEFAULT: return input;
	case PM_LOGLOG: {
		assert (input > 0);
		return log10(input);
	}
	}
	// cannot come here!
	return -1;
}

/**
 * Draw a PPM figure. This is a colour plot with length and width sqrt(data->size). It
 * expects values in the range [0..1] and will multiple them by 255 to obtain chars.
 * Subsequently they are put into four bins, each given a certain main colour, but with
 * the colours gradually changing from bin to bin.
 */
void Plot::DrawPPM() {
	FILE *stream;
	string file = path + ppm_file + ".ppm";
	stream = fopen(file.c_str(), "wb" );
	int len = sqrt(GetData().size());
	fprintf( stream, "P6\n%d %d\n255\n", len, len);

	int sections = 4; int nof_colors = sections*255;
	char rgb[3];
	for(int j = 0; j < len; ++j) {
		for (int i = 0; i < len; ++i) {
			float value = GetData().item< float >(i+j*len) * nof_colors;
			if(value < 0) {
				rgb[0] = 0;
				rgb[1] = 0;
				rgb[2] = 0;
			} else
				if(value < 256)
				{
					rgb[0] = 0;
					rgb[1] = value; // 0 b is bluest, and up to g+b=cyan
					rgb[2] = 255;
				}
				else if(value < 511)
				{
					rgb[0] = 0;
					rgb[1] = 255;
					rgb[2] = 511 - value; // 255 is g+b=cyan, 511 g is greenest
				}
				else if (value < 766)
				{
					rgb[0] = value - 511;
					rgb[1] = 255; // 511 g is greenest, 765 is r+g=yellow
					rgb[2] = 0;
				}
				else if (value < 1021)
				{
					rgb[0] = 255;
					rgb[1] = 1020 - value; // 765 is r+g=yellow, 1020 is reddest
					rgb[2] = 0;
				}
			// 511 is r+g=yellow, 765 is reddest
			fwrite(&rgb[0], 1, 1, stream);
			fwrite(&rgb[1], 1, 1, stream);
			fwrite(&rgb[2], 1, 1, stream);
		}
	}
	fclose( stream );
}

/**
 * Get the data from the given container.
 */
void Plot::GetData(DataContainer &cont, PLData & pld) {
	pld.len = cont.size();
	if (pld.len == 0) {
		cerr << "No data available!" << endl;
		return;
	}
	pld.x_axis = new PLFLT[pld.len];
	pld.y_axis = new PLFLT[pld.len];
	assert (cont.GetID() >= 0);
	pld.id = cont.GetID();

	switch (plot_type) {
	case PT_DEFAULT:
		//		cout << "Plot values" << endl;
		for (int i = 0; i < pld.len; ++i) {
			pair<DataDecoratorType,int> item = cont.item< pair<DataDecoratorType,int> >(i);
			DataDecoratorType x = item.first;
			int y = item.second;
			pld.x_axis[i] = Scale(x, true);
			pld.y_axis[i] = Scale(y, false);
		}
		break;
	case PT_DENSITY:
	case PT_CUMULATIVE_DENSITY:
		// Total number of samples
		long int N = 0;
		for (int i = 0; i < pld.len; ++i) {
			pair<DataDecoratorType,int> item = cont.item< pair<DataDecoratorType,int> >(i);
			int y = item.second;
			N += y;
		}
#ifdef VERBOSE
		cout << "Total number of samples is " << N << endl;
#endif
		if (N == 0) break;

		// calculate cumulative density function
		bool reverse_cdf = false;
		long int sum = 0;
		for (int i = 0; i < pld.len; ++i) {
			int index = (reverse_cdf ? pld.len - 1 - i : i);
			pair<DataDecoratorType,int> item = cont.item< pair<DataDecoratorType,int> >(index);
			DataDecoratorType x = item.first;
			int y = item.second;
			if (plot_type == PT_DENSITY) {
				PLFLT delta = 1;
				pld.x_axis[index] = Scale(x, true);
				if (reverse_cdf) {
					if (index != pld.len-1) delta = pld.x_axis[index+1] - pld.x_axis[index];
				} else
					if (index != 0) delta = pld.x_axis[index-1] - pld.x_axis[index];
				sum = y;
				pld.y_axis[index] = Scale((sum / (float)N) / delta, false);
			} else {
				sum += y;
				pld.x_axis[index] = Scale(x, true);
				pld.y_axis[index] = Scale(sum / (float)N, false);
			}
		}
	}

	// For debugging purposes
	if (plot_type == PT_DENSITY) {
		PLFLT lazy_integration = 0;
		for (int i = 1; i < pld.len; ++i) {
			PLFLT delta = pld.x_axis[i] - pld.x_axis[i-1];
			lazy_integration += pld.y_axis[i] * delta;
		}
		cout << "Integration of density plot: " << lazy_integration << " (should be around 1)" << endl;
	}

	if (!dimensions_set) {
		pld.x_min = *min_element(pld.x_axis,pld.x_axis+pld.len);
		pld.x_max = *max_element(pld.x_axis,pld.x_axis+pld.len);
		pld.y_min = *min_element(pld.y_axis,pld.y_axis+pld.len);
		pld.y_max = *max_element(pld.y_axis,pld.y_axis+pld.len);
	}

	// Remove all values smaller than 1/resolution of the maximum value
	bool remove_below_resolution = false;
	int resolution = 1000;
	if (remove_below_resolution) {
		int j = 0;
		for (int i = 0; i < pld.len; ++i) {
			PLFLT x = pld.x_axis[i];
			PLFLT y = pld.y_axis[i];
			if (!dimensions_set) {
				if ((y * resolution) > pld.y_max) {
					pld.x_axis[j] = x;
					pld.y_axis[j] = y;
					j++;
				}
			} else {
				if (y > y_min) {
					pld.x_axis[j] = x;
					pld.y_axis[j] = y;
					j++;
				}
			}
		}
		pld.len = j;

		if (!dimensions_set) {
			pld.x_min = *min_element(pld.x_axis,pld.x_axis+pld.len);
			pld.x_max = *max_element(pld.x_axis,pld.x_axis+pld.len);
			pld.y_min = *min_element(pld.y_axis,pld.y_axis+pld.len);
			pld.y_max = *max_element(pld.y_axis,pld.y_axis+pld.len);
		}
	}
}

/**
 * Go over all the data (retrieve it from the containers). Then calculate the maximum
 * dimensions for the plot. Calculate other relevant parameters and call the proper
 * PLPlot functions. Finally draw for every container a separate graph.

 * @todo A lot... First of all, the commands in the form of letters to pls->box
 *   should each be attached to a corresponding function in a PlotOpenPL class.
 */
void Plot::DrawPLPlot() { //bool data2file, bool file2data) {
	if (data_v.empty()) {
		cerr << "No data available, has SetData been called?" << endl;
		return;
	}

	std::vector<PLData> plds;

	std::vector<DataContainer*>::iterator d_i;
	for (d_i = data_v.begin(); d_i != data_v.end(); ++d_i) {
		if (plot_type == PT_DENSITY) {
			cerr << "Warning: applied bins with fixed sizes..." << endl;
			// If you use ApplyBins, please, take notice that this will not
			// set the x-axis min and max values, for that you will have to set
			// boundaries explicitly with SetDimensions
			double x_min = 0;
			double x_max = 4;
//			SetDimensions(x_min, x_max, 0, 0.8);
			(*d_i)->ApplyBins(100, x_min, x_max);
//			(*d_i)->write(std::cout);
		}
		PLData pld;
		GetData(**d_i, pld);
		if (pld.len == 0) {
			cerr << "No data available, has SetData been called?" << endl;
			continue;
		}
		if (pld.len == 1) {
			cerr << "Just one data point. Does not make sense to make a plot!" << endl;
			continue;
		}
		plds.push_back(pld);
	}

	if (plds.empty()) {
		cerr << "No data available, empty files?" << endl;
		return;
	}

	// Calculate minimum and maximum of x and y over all plots
	PLFLT lx_min, lx_max, ly_min, ly_max;
	if (dimensions_set) {
		lx_min = x_min;
		lx_max = x_max;
		ly_min = y_min;
		ly_max = y_max;
	} else {
		PLData & pld = plds.front();
		lx_min = pld.x_min;
		lx_max = pld.x_max;
		ly_min = pld.y_min;
		ly_max = pld.y_max;

		std::vector<PLData>::iterator p_i;
		for (p_i = plds.begin(); p_i != plds.end(); ++p_i) {
			PLData & pld = *p_i;
			if (pld.x_min < lx_min) lx_min = pld.x_min;
			if (pld.x_max > lx_max) lx_max = pld.x_max;
			if (pld.y_min < ly_min) ly_min = pld.y_min;
			if (pld.y_max > ly_max) ly_max = pld.y_max;
		}
	}

	pls->adv( 0 );

	// Dimensions of the plot in "screen coordinates"
	pls->vpor( 0.15, 0.85, 0.1, 0.9 );

	PLFLT x_border = (lx_max - lx_min) / 5;
	PLFLT y_border = (ly_max - ly_min) / 5;
#ifdef VERBOSE
	cout << "With x_max (" << lx_max << ") - x_min (" << lx_min << ") x_border becomes " << x_border << endl;
#endif
	pls->wind( lx_min - x_border, lx_max + x_border, ly_min - y_border, ly_max + y_border);

	switch (plot_type) {
	case PT_DEFAULT:
		break;
	case PT_DENSITY:
		y_label = y_label + " (pdf)";
		break;
	case PT_CUMULATIVE_DENSITY:
		y_label = y_label + " (cdf)";
		break;
	}

	// Set colour of axes
	pls->col0( DeepBlue );

	switch ( plot_mode )
	{
	case PM_DEFAULT:
		pls->box( "bcnst", 0.0, 0, "bnstv", 0.0, 0 );
		break;
	case PM_SEMILOG:
		pls->box( "bcnst", 0.0, 0, "blnstv", 0.0, 0 );
		break;
	case PM_LOGLOG:
		pls->box( "bclnst", 0.0, 0, "blnstv", 0.0, 0 );
		break;
	}

//	PLcolor
	int colour = Black;
	int style = 1;
	char sign = '.'; // Plot as '.' or 'o' or 'x'

	// Plot as line
	bool plot_as_line = true;

	// Add labels (and the legend)
	pls->col0( DeepBlue );
	pls->mtex( "b", 3.2, 0.5, 0.5, x_label.c_str() );
	pls->mtex( "t", 2.0, 0.5, 0.5, title_label.c_str() );
	pls->mtex( "l", 5.0, 0.5, 0.5, y_label.c_str() );

    int legend_id = 1;
	// Actually do the drawing
	std::vector<PLData>::iterator p_i;
	for (p_i = plds.begin(); p_i != plds.end(); ++p_i) {
		PLData & pld = *p_i;

		// Set colour of the line to the next one
		colour+=1;
		// Skip yellow for visibility - goes from red to green
		if (colour == Yellow) colour +=1;
		// Skip white and go to black again..
		colour %= White;
		pls->col0( colour);

//		if (plot_type == PT_DENSITY) {
//			pls->hist( pld.len, pld.y_axis, pld.x_min, pld.x_max, 10, 1);
//		} else {
			if (plot_as_line) {
				pls->lsty( style );
				pls->line( pld.len, (PLFLT*)pld.x_axis, (PLFLT*)pld.y_axis );
			} else {
				pls->poin( pld.len, pld.x_axis, pld.y_axis, sign );
			}
//		}

		string y_run = y_label + " [" + boost::lexical_cast<std::string>(pld.id) + "]";
		pls->mtex( "t", -(++legend_id*1.5), 0.9, 0.0, y_run.c_str() );
	}

	// Remove arrays
	for (p_i = plds.begin(); p_i != plds.end(); ++p_i) {
		PLData & pld = *p_i;
		delete [] pld.x_axis;
		delete [] pld.y_axis;
	}
}

/**
 * Store the data for the files
 */
void Plot::Store() {
	std::vector<DataContainer*>::iterator d_i;
	for (d_i = data_v.begin(); d_i != data_v.end(); ++d_i) {
		string pfile = path + svg_file + ".data";
		ofstream ofile;
		ofile.open(pfile.c_str());
		(*d_i)->write(ofile);
		ofile.close(); //! need to close it, or we can't read from it

		bool read_back = false;
		if (read_back) {
			(*d_i)->clear();
			ifstream ifile(pfile.c_str());
			if ( ifile.is_open() ) {
				(*d_i)->read(ifile);
			} else {
				cerr << "Plot: couldn't open file" << endl;
			}
		}
	}
}
