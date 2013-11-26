#include "stdafx.h"
#include "Stream.h"

/** 
 * Default constructor
 * @param int	The id of camera
 */
Stream::Stream()
{
	
}

/** 
 * Method is used to create a directory for saving streamed data
 * @return void
 */
void Stream::mkdir()
{
	char tmp[100];
	sprintf(tmp, CFG_STORAGE, _camera_id);
	_mkdir(tmp);
	strcat(tmp, this->getStoragePrefix());
	_mkdir(tmp);
}

/** 
 * Method is used to get the storage prefix for particular sensor type
 * @return char *
 */
char * Stream::getStoragePrefix()
{
	char *tmp = new char[40];
	switch(this->getSensorInfo().getSensorType())
	{
		case SENSOR_COLOR:
			strcpy(tmp, "RGB/");
			break;
		case SENSOR_DEPTH:
			strcpy(tmp, "Depth/");
			break;
		case SENSOR_IR:
			strcpy(tmp, "IR/");
			break;
	}

	return tmp;
}

/**
 * Method is used to capture the next frame
 * @return VideoFrameRef	Object containing the next frame
 */
VideoFrameRef Stream::nextFrame()
{
	int changedIndex;
	VideoStream* pStream = this;
	OpenNI::waitForAnyStream(&pStream, 1, &changedIndex, 2000);
	VideoFrameRef nextFrame;
	this->readFrame(&nextFrame);
	return nextFrame;
}

void Stream::saveFrame(Mat frame)
{
	IplImage img = (IplImage) frame;
	char *tmp = new char[100];
	sprintf(tmp, CFG_STORAGE, _camera_id);
	strcat(tmp, this->getStoragePrefix());
	strcat(tmp, to_string(path_buffer.size()).c_str());		
	strcat(tmp, CFG_IMAGE_EXTENTION);
	cvSaveImage(tmp, &img);
	path_buffer.push_back(tmp);
}

/** 
 * Method is used to covert VideoFrameRef to OpenCV mat
 * @param VideoFrameRef	The instance of VideoFrameRef needs to be converted
 * @param bool	If passed true, the image also will be blured, otherwise will return non-blurred version
 * @return Mat
 */
Mat Stream::frameToMat(VideoFrameRef frameRef, bool blur)
{
	Mat *frame = NULL;
	switch(frameRef.getSensorType())
	{
		case SENSOR_COLOR:
			frame = new Mat(frameRef.getHeight(), frameRef.getWidth(), CV_8UC3, (unsigned char*) frameRef.getData());
			cvtColor(*frame, *frame, CV_BGR2RGB);
			break;
		case SENSOR_DEPTH:
			frame = new Mat(frameRef.getHeight(), frameRef.getWidth(), CV_16UC1, (unsigned char*) frameRef.getData());			
			break;
		case SENSOR_IR:
			frame = new Mat(frameRef.getHeight(), frameRef.getWidth(), CV_8U, (unsigned char*) frameRef.getData());		
			break;
	}

	if(blur)
	{
		medianBlur(*frame, *frame, 5);
	}

	return *frame;
}

/**
 * Method is used to write the paths of captured images in file for furture use
 * @return	bool 
 */
bool Stream::saveFilePaths()
{
	ofstream file;
	char tmp[100];
	sprintf(tmp, CFG_STORAGE, _camera_id);
	strcat(tmp, this->getStoragePrefix());
	strcat(tmp, "images.paths");
	file.open(tmp);
	if(!file) {
		//cerr << "Error: file could not be opened" << endl;
		return false;
    }

	vector<string>::iterator it;
	for(it = path_buffer.begin(); it != path_buffer.end(); ++it)	
	{
		file<<(*it)<<"\n";
	}

	file.close();
	return true;
}

/**
 * Method is used to read the paths of captured images from the file
 * @return	bool 
 */
bool Stream::loadFilePaths()
{
	ifstream file;
	char tmp[100];
	sprintf(tmp, CFG_STORAGE, _camera_id);
	strcat(tmp, this->getStoragePrefix());
	strcat(tmp, "images.paths");
	file.open(tmp);

	if(!file) {
      cerr << "Error: file could not be opened" << endl;
      return false;
    }

	path_buffer.clear();
	string line;
	while (getline(file, line)){
		path_buffer.push_back(line);
	}

	file.close();
	return true;
}

/** 
 * Method is used to get the buffer containing the paths
 * @return vector<string> &
 */
vector<string> & Stream::getPathBuffer()
{
	return this->path_buffer;	
}