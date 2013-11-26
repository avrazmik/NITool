#pragma once
#include <OpenNI.h>
#include <opencv/highgui.h>
#include <direct.h>
#include <opencv/cv.h>
#include <fstream>
#include <sstream>
#include "inc.h"

using namespace std;
using namespace openni;
using namespace cv;

class Stream: public VideoStream
{
public:
	Stream();
	void mkdir();
	char * getStoragePrefix();
	VideoFrameRef nextFrame();
	void saveFrame(Mat);
	Mat frameToMat(VideoFrameRef, bool = false);
	int _camera_id;
	bool saveFilePaths();
	bool loadFilePaths();
	vector<string> & getPathBuffer();
private:
	vector<string> path_buffer;
};