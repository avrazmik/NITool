#pragma once
#include <Windows.h>
#include <string.h>
#include <libMesaSR.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <conio.h>
#include <pthread.h>
#include "ColorModel.h"
#include "DepthModel.h"
#include "LookupGL.h"

using namespace std;

class MesaSR: public DepthModel, public ColorModel
{
public:
	MesaSR(CMesaDevice *);
	bool calibrateDepth();
	static void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);
	CMesaDevice* getDevice();
	static void *glut_vis(void *ptr);
	int rows;
	int cols;
private: 
	CMesaDevice* srCam;	
};

