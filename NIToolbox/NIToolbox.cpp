// NIToolbox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>

#include <GL/glut.h>
#include "NITool.h"
#include "MesaSR.h"
#include "OptiTrack.h"
#include "Lookup.h"
#include "LookupGL.h"

using namespace cv;
using namespace openni;

#pragma warning( disable : 4996 )

NITool *oNI;

int main(int argc, CHAR* argv[])
{
	oNI = NITool::getInstance();
	try {
		oNI->run();
	}
	catch (exception e) {
		cerr<<e.what();
	}
	
	oNI->finalize();
	return 0;
}
