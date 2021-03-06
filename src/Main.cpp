/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/

#include <string>

#include "AgileEvtReader.h" // -> imports EvtReader too
#include "CtaEvtReader.h" // -> imports EvtReader too
#include "HealpixParams.h" // to remove
#include <PilParams.h>
#include "HealpixMapMaker.h"
#include "FileWriter.h"


using namespace std;

const char* startString = {
"################################################################\n"
"###             Task PhotonList2Healpix v0.0.1               ###\n"
};

const char* endString = {
"\n### Task PhotonList2Healpix exiting ......................... ###\n"
"#################################################################"
};


const PilDescription paramsDescr[] = {
    { PilString, "outfile", "Output file name" },
    { PilString, "evtType", "Event telescope source" },
    { PilString, "photonListPath", "Path of photon list"},
   
    
    //i successivi 5 sono relativi alla proiezione nel cielo
    { PilReal, "mdim", "Size of Map (degrees)" },
    { PilReal, "mres", "Heaplix resolution (level)" },
    { PilReal, "la", "Longitude of map center (galactic)" },
    { PilReal, "ba", "Latitude of map center (galactic)" },
    { PilReal, "lonpole", "Rotation of map (degrees)" },

    { PilReal, "albrad", "Radius of earth albedo (degrees)" },
    { PilInt, "phasecode", "Orbital phase code" },
    { PilInt, "filtercode", "Event filter code" },
    { PilReal, "tmin", "Initial time(sec)" },
    { PilReal, "tmax", "Final time(sec)" },
    { PilReal, "emin", "Min energy" },
    { PilReal, "emax", "Max energy" },
    { PilReal, "fovradmin", "Min off-axis angle (degrees)" },
    { PilReal, "fovradmax", "Max off-axis angle (degrees)" },
    { PilNone, "", "" }
};


int main(int argc, char *argv[])
{
    cout << startString << endl;
    
    PilParams params(paramsDescr);
    
    if (!params.Load(argc, argv))
        return EXIT_FAILURE;
        
	cout << endl << "INPUT PARAMETERS:" << endl;
    params.Print();
    
 	
    char selectionFilename[FLEN_FILENAME];
    char templateFilename[FLEN_FILENAME];
    tmpnam(selectionFilename);
    tmpnam(templateFilename);
    
    
    int status;
    
    Intervals intervals;
 	Interval intv(params["tmin"],params["tmax"]);
    intervals.Add(intv);
	
	// Data to insert in EVT.index
	const char * evtFile = "./INDEX/EVT.index";	
	const char * _photonListPath = params["photonListPath"];
	double _tmin = params["tmin"];
	double _tmax = params["tmax"];
	
	// Create EVT.index	
	string _pLP(_photonListPath);
	string input2write = _pLP +" "+ to_string(_tmin) + " " + to_string(_tmax);
	
	FileWriter :: write2File(evtFile,input2write);
    
    cout << "EVT file created"<< endl;
	
	
	EvtReader * evtReader;
	EvtParams * readerParams; // emin, emax, phasecode, filtercode, tmin, tmax

		
	HealpixParams healpix2WriteParams(params["mdim"],params["mres"],params["la"],params["ba"], params["lonpole"]);

	healpix2WriteParams.print();

	
	string _evtType (params["evtType"]);
	
		
	if( _evtType == "AGILE")
	{
		cout << "AGILE selected" << endl;

		evtReader    = new AgileEvtReader();

		readerParams  = new AgileEvtParams( 	evtFile,
							params["emin"],
							params["emax"],
							params["albrad"],
							params["fovradmin"],
							params["fovradmax"],
							params["phasecode"],
							params["filtercode"],
							params["tmin"],
							params["tmax"]
						);
	}
	else if( _evtType == "CTA")
	{
		cout << "CTA selected" << endl;

		evtReader    = new CtaEvtReader();

		readerParams = new CtaEvtParams(	evtFile,
							params["emin"],
							params["emax"],
							params["tmin"],
							params["tmax"]
						);
	}
	

	evtReader->readEvtFile(selectionFilename, templateFilename,readerParams);
	
	status = HealpixMapMaker :: EvalCountsHealpix(	params["outfile"],
							evtReader, 
							readerParams, 
							healpix2WriteParams, 
							selectionFilename, 
							templateFilename, 
							intervals
						);
	cout << "Healpix status: " << status;
	cout << endString << endl;
	return 0;
}	

