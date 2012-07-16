/**
 * @file Plot.h
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


#ifndef PLOT_H_
#define PLOT_H_

// General files
#include <plplot/plstream.h>
#include <string>
#include <map>
#include <vector>
#include <DataDecorator.h>

/* **************************************************************************************
 * Interface of Plot
 * **************************************************************************************/

enum PlotMode { PM_DEFAULT, PM_SEMILOG, PM_LOGLOG };

enum OutputType { PLPLOT, PPM };

enum PlotType { PT_DEFAULT, PT_DENSITY, PT_CUMULATIVE_DENSITY };

struct PLData {
	PLFLT *x_axis;
	PLFLT *y_axis;
	int len;
	PLFLT x_min;
	PLFLT x_max;
	PLFLT y_min;
	PLFLT y_max;
	int id;
};

class Plot {
public:
	//! Constructor Plot
	Plot();

	//! Destructor ~Plot
	virtual ~Plot();

	//! Initialisation
	void Init(OutputType outputType);

	//! Actually draw the plot
	void Draw(OutputType outputType); //, bool data2file = true, bool file2data = true);

	//! Store the data to file, so we can plot later again
	void Store();

	//! Title on top
	inline void SetTitle(const std::string & title) { title_label = title; }

	//! Title on the horizontal axis
	inline void SetXAxis(const std::string & label) { x_label = label; }

	//! Title on the vertical axis (left)
	inline void SetYAxis(const std::string & label) { y_label = label; }

	//! Plot mode (linear, semilog, loglog)
	inline void SetPlotMode(PlotMode pm) { plot_mode = pm; }

	//! Plot type (default, pdf, cdf)
	inline void SetPlotType(PlotType pt) { plot_type = pt; }

	//! Get the data
	DataContainer & GetData(int id = -1);

	//! Set filename (use also SetPath)
	void SetFileName(std::string filename, OutputType outputType);

	//! Set working directory for output and input
	void SetPath(std::string path);

	inline void SetDimensions(double x_min, double x_max, double y_min, double y_max) {
		dimensions_set = true;
		this->x_min = x_min;
		this->x_max = x_max;
		this->y_min = y_min;
		this->y_max = y_max;
	}
protected:
	//! @todo Move to subclass
	void DrawPLPlot(); //bool data2file = true, bool file2data = true);

	//! @todo Move to subclass
	void DrawPPM();

	//! Scale depending on the mode
	PLFLT Scale(const PLFLT input, bool x_axis=true);

	//! Get data from container into arrays
	void GetData(DataContainer &cont, PLData & pld);

private:
	//! Multiple data containers
	std::vector<DataContainer*> data_v;

	//! Current container
	int current_container;

	//! Plot mode
	PlotMode plot_mode;

	//! Plot type
	PlotType plot_type;

	//! File name for .ppm file
	std::string ppm_file;

	//! File name for .svg file
	std::string svg_file;

	//! Working dir
	std::string path;

	//! PlPlot stream
	plstream *pls;

	//! Type of graph (rasters etc)
//	int type;

	//! X label (horizontal at the bottom)
	std::string x_label;

	//! Y label at the left side
	std::string y_label;

	//! Y label at the right side
	std::string alty_label;

	//! Y label at the right side
	std::string title_label;

	//! We can even introduce labels to the lines we draw
	std::string line_label;

	//! Dimensions set by the user (so we do not need to calculate them)
	bool dimensions_set;

	//! Dimensions themselves
	PLFLT x_min, x_max, y_min, y_max;
};

#endif /* PLOT_H_ */
