#pragma once
#define CFG_KINECT_STORAGE "C:/Users/Razmik/Desktop/opencv_images/%u/"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <OpenNI.h>
#include <iostream>
#include <stdlib.h>
#include <map>
#include "inc.h"
#include "Kinect.h"

enum NI_MODE{MODE_CAPTURE = '1', MODE_CALIBRATION = '2',  MODE_UNDISORT = '3', MODE_EXPORT = '4'};

using namespace std;
using namespace openni;
using namespace cv;

class NITool 
{
public:
	static NITool * getInstance();
	void setImagesPerCamera(int);
	int getImagesPerCamera();
	void setParallelCapture(bool);
	bool getParallelCapture();
	void setSensorType(SensorType);
	SensorType getSensorType();
	void run();
	void finalize();
	map<int, Kinect *> & getDevices();	
	size_t getDeviceCount();
	bool displayMultiFrame(const char *, vector<IplImage>, const SensorType &);
	void printHelp();
	void captureAction();
	void calibrationAction();
	void exportAction();
	void undistortAction();
private:
	NITool();
	static NITool * instance;
	int cfg_images_per_camera;
	bool cfg_parallel_capture;
	SensorType cfg_sensor_type;
	map<int, Kinect *> devices;
	Array<DeviceInfo> deviceList;
};

