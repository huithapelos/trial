//-----------------------------------------------------------------------------
/// \file er2par.cpp
/// \brief Ermapper vector to pattern polygon
//
///  $Author: jani $
///  $Log: er2pat.cpp,v $
///  Revision 1.1.1.1  2007/03/08 08:17:22  jani
///  no message
///
//
//   Copyright 2006 PIEneering Oy. All rights reserved.
//-----------------------------------------------------------------------------

#include <vector>
#include <pcl.hpp>
#include <pcl_tmp.hpp>
#include <cmdline.hpp>
#include <inifile.hpp>
#include <message.hpp>
#include <filename.hpp>

using namespace pcl;
using namespace std;

const PcString versionNro("1.00");

const PcString argErm("-e");
const PcString argPixelsize("-pixelsize");

CBool output_converted(const PcFileName &ervFile,
											 CFloat64 pixelsize, PcString &ss);
void show_welcome();
PcString get_usage();

int main(int argc, char *argv[]) 
{
	try 
	{
		PcMessage msg;

		PcArgumentMap arguments(argc, argv);
		if ( arguments.isKey("-?") || 
				 arguments.isKey("-h") || arguments.isKey("-help") ) 
		{
			std::cout << get_usage() << std::endl;
			return 0;
		}
		show_welcome();

		/*
		 * Input
		 */
		PcFileName ervFile(arguments.getCritical(argErm));
		CFloat64 pixelsize = arguments.get(argPixelsize,20.0);
		pixelsize *= 0.001;

		PcString ss;
		if ( !output_converted(ervFile,pixelsize,ss) ) {
			cout << msg.errors();
			cout << "er2pat failed";
		}
		else {
			cout << "er2pat successfull\n";
			if ( msg.isError() ) {
				cout << ".. but reports un-catched errors as follows\n";
				cout << msg.errors();
			}

			cout << "Data\n";
			cout << ss << "\n";
		}

		return 0;
	}
	
	catch(PcException &e) 
	{
		// Output the usage to stdout (the error description is already printed).
		cout << e.what();
		std::cout << std::endl << get_usage() << std::endl ;
		return 1;
	}
}

PcString get_usage()
{
	PcString ss = "er2pat\n";
	ss += "\t" + argErm + " erv-file path\n";
	ss += "\t" + argPixelsize + " Digitization pixelsize in micrometers\n";
	return ss;
}

void show_welcome()
{
	std::cout << "\ner2pat v." << versionNro << "\n";
	std::cout << ", Copyright PIEneering Oy 2006." << std::endl;
}

CBool output_converted(const PcFileName &ervFile,
											 CFloat64 pixelsize, PcString &ss)
{
	PcFileName vecFile(ervFile);
	vecFile.removeExtension();
	if ( !vecFile.exists() )
		PcMessage(Fatal,"Data file does not exists " + vecFile);

	PcString contents;
	if ( !contents.setFromFile(vecFile) )
		PcMessage(Fatal,"Could not read data file " + vecFile);

	//reference
	string::size_type pos;
	if ( (pos = contents.find("point")) == string::npos )
		PcMessage(Fatal,"No reference point digitized in " + vecFile);

	PcString pstr;
	if ( (pos = contents.pick('(',')',pstr,pos)) == string::npos )
		PcMessage(Fatal,"Reference point data illegal in " + vecFile);

	pstr.findReplace(PcString(","),PcString(" "));
	vector<PcString> ww;
	pstr.splitToWords(ww);

	CFloat64 x0 = ww[0].float64();
	CFloat64 y0 = ww[1].float64();

	if ( (pos = contents.find("poly")) == string::npos )
		PcMessage(Fatal,"No polyline features digitized in " + vecFile);

	if ( (pos = contents.pick('[',']',pstr,pos)) == string::npos )
		PcMessage(Fatal,"Polyline data illegal in " + vecFile);

	CUInt32 iw,nw = pstr.splitToWords(ww);
	if ( nw%2 != 0 ) 
		PcMessage(Fatal,"Odd number of coordinates in polyline in " + vecFile);
	
	ostringstream oss;
	oss.setf(ios_base::fixed);
	oss.precision(3);
	for ( iw=0; iw<nw; iw+=2 )
	{
		CFloat64 x1 = ww[iw].float64();
		CFloat64 y1 = ww[iw+1].float64();
		oss << (x1-x0)*pixelsize << "," << (y0-y1)*pixelsize << ";";
	}
	ss = oss.str();

	return true;
}
