#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <OpenNI.h>
#include <string>
#include <direct.h>
#include <math.h>
#include <pthread.h>
#include "inc.h"
#include "SensorIR.h"
#include "SensorRGB.h"
#include "Stream.h"
#include "OptiTrack.h"
#include "LookupGL.h"

using namespace std;
using namespace openni;
using namespace cv;

class Kinect: public Device{
public:
	Kinect(int id, const char* uri = NULL);
	int getCameraId();
	SensorIR * getSensorIR();
	SensorRGB * getSensorRGB();
	Stream * getStream(SensorType type);
	Stream * startStream(SensorType type);
	void stopStream(SensorType type);
	void stopAllStreams();
	bool calibrate(char, char);
	bool calibratePair();
	bool calibrateDepth();
	static void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);
	static void * Kinect::glut_loop( void *ptr );
	static void *glut_vis(void *ptr);
private:
	int camera_id;
	SensorIR *ir;
	SensorRGB *rgb;
};