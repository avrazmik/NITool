#include "stdafx.h"
#include "NITool.h"

NITool* NITool::instance  = NULL;

/** 
 * This method is used to get the instance of NITool - singleton pattern
 * @return Pointer to instance of the NITool
 */
NITool * NITool::getInstance()
{
	if (instance == NULL){
		instance = new NITool(); 
	}

	return instance;
}

/** 
 * Default constructor 
 */
NITool::NITool(){}

/** 
 * Method is used to set the number of images per camera
 * @param int	Number of images per camera
 * @return void
 */
void NITool::setImagesPerCamera(int n)
{
	cfg_images_per_camera = n;
}

/** 
 * Method is used to return the number of images per camera
 * @return int	Number of images per camera
 */
int NITool::getImagesPerCamera()
{
	return cfg_images_per_camera;
}

/** 
 * Method is used to set the boolean flag parallel capture
 * @param bool	If set true all cameras will start capturing parallel, otherwise in sequential order
 * @return void
 */
void NITool::setParallelCapture(bool flag)
{
	cfg_parallel_capture = flag;
}

/** 
 * Method is used to get the boolean flag parallel capture
 * @return bool
 */
bool NITool::getParallelCapture()
{
	return cfg_parallel_capture;
}

/** 
 * Method is used to handle image capture action
 * @return void
 */
void NITool::captureAction()
{	
	int n_images = 0;
	string str_n_images;
	while(n_images <= 0)
	{
		cout<<"\nNumber of images per camera: ";
		cin>>str_n_images;
		n_images = atoi(str_n_images.c_str());
		if(n_images <= 0)
		{
			printf("Invalid input %s: a positive value is required.\n\n", str_n_images.c_str());
			continue;
		}
		
		printf("OK!\n");
		this->setImagesPerCamera(n_images);
	}

	char parallel_capture;
	if(deviceList.getSize() < 2) 
	{
		parallel_capture = 'n';
	} 
	
	else 

	{
		parallel_capture = '0';	
		while(parallel_capture != 'y' && parallel_capture != 'n')
		{
			cout<<"\nParallel capture mode (y/n): ";
			cin>>parallel_capture;

			if(parallel_capture != 'y' && parallel_capture != 'n')
			{
				printf("Invalid input: (y/n) is required.\n\n");
				continue;
			}

			printf("OK!\n");			
		}
	}

	this->setParallelCapture(parallel_capture == 'y' ? true : false);

	char sensor_type = ' ';
	while(sensor_type != NI_SENSOR_TYPE_RGB && sensor_type != NI_SENSOR_TYPE_IR)
	{
		printf("\nType of sensor to use (%c-color/%c-ir): ", NI_SENSOR_TYPE_RGB, NI_SENSOR_TYPE_IR);
		cin>>sensor_type;

		if(sensor_type != NI_SENSOR_TYPE_RGB && sensor_type != NI_SENSOR_TYPE_IR)
			continue;
			
		printf("OK!\n");
	}

	switch(sensor_type) 
	{
		case NI_SENSOR_TYPE_RGB: 
			this->setSensorType(SENSOR_COLOR);
			break;
		case NI_SENSOR_TYPE_IR:
			this->setSensorType(SENSOR_IR);
			break;
	}
	
	map<int, Kinect *>::iterator it;
	for (it = this->devices.begin(); it != this->devices.end(); it++)
	{
		(*it).second->stopAllStreams();
		(*it).second->startStream(this->getSensorType());
	}

	bool blur = this->getSensorType() == SENSOR_IR ? true : false;
	Stream *tmpStream;
	IplImage img;
	if(this->getParallelCapture())
	{
		int j=1;
		cvNamedWindow("NIToolbox");
		
		if(this->getParallelCapture())
			cvMoveWindow("NIToolbox", 10, 20);
			
		while(j <= this->getImagesPerCamera())
		{
			vector<IplImage> vec;
			for (it = this->devices.begin(); it != this->devices.end(); it++)
			{
				tmpStream = (*it).second->getStream(this->getSensorType());
				Mat frame = tmpStream->frameToMat(tmpStream->nextFrame(), blur);
				tmpStream->saveFrame(frame);
				img = (IplImage) frame;
				vec.push_back(img);
			}

			this->displayMultiFrame("NIToolbox", vec, this->getSensorType());
			j++;
		}

		cvDestroyWindow("NIToolbox");
	}
	
	else

	{
		
		for (it = this->devices.begin(); it != this->devices.end(); it++)
		{
			int j=1;
			while(j <= this->getImagesPerCamera())
			{
				tmpStream = (*it).second->getStream(this->getSensorType());
				Mat frame = tmpStream->frameToMat(tmpStream->nextFrame(), blur);
				tmpStream->saveFrame(frame);
				img = (IplImage) frame;
				cvShowImage("NIToolbox", &img);
				cout<<'\a';

				cvWaitKey(CFG_CAPTURE_INTERVAL);
				j++;
			}

			cvDestroyAllWindows();
		}
	}

	for (it = this->devices.begin(); it != this->devices.end(); it++)
	{
		(*it).second->getStream(this->getSensorType())->saveFilePaths();
		(*it).second->getStream(this->getSensorType())->getPathBuffer().clear();
		(*it).second->stopAllStreams();
	}

	printf("OK!\n");
}

/** 
 * Method is used to handle calibration action
 * @return void
 */
void NITool::calibrationAction()
{
	char calibration_type = ' ';
	while( calibration_type != '0'
		&& calibration_type != NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC 
		&& calibration_type != NI_CALIBRATION_TYPE_INTRINSIC 
		&& calibration_type != NI_CALIBRATION_TYPE_EXTRINSIC
		&& calibration_type != NI_CALIBRATION_TYPE_DEPTH
		&& calibration_type != NI_CALIBRATION_TYPE_PAIR)
	{
		printf("\n**********\n");
		printf("enter %c to perform intrinsic and extrinsic calibration\n", NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC);
		printf("enter %c to perform intrinsic calibration\n", NI_CALIBRATION_TYPE_INTRINSIC);
		printf("enter %c to perform extrinsic calibration\n", NI_CALIBRATION_TYPE_EXTRINSIC);
		printf("enter %c to perform depth calibration\n", NI_CALIBRATION_TYPE_DEPTH);
		printf("enter %c to perform build-in sensor calibration\n", NI_CALIBRATION_TYPE_PAIR);
		printf("enter 0 to return\n");
		printf("\n>> ");
		cin>>calibration_type;
	}

	char sensor_type = ' ';
	if(calibration_type == NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC || calibration_type == NI_CALIBRATION_TYPE_INTRINSIC || calibration_type == NI_CALIBRATION_TYPE_EXTRINSIC)
	{
		while(sensor_type != NI_SENSOR_TYPE_RGB && sensor_type != NI_SENSOR_TYPE_IR)
		{
			printf("\nType of sensor to calibrate (%c-color/%c-ir): ", NI_SENSOR_TYPE_RGB, NI_SENSOR_TYPE_IR);
			cin>>sensor_type;

			if(sensor_type != NI_SENSOR_TYPE_RGB && sensor_type != NI_SENSOR_TYPE_IR)
				continue;
			
			printf("OK!\n");
		}
	}

	if(calibration_type == NI_CALIBRATION_TYPE_DEPTH)
	{
		this->devices[10000]->calibrate(calibration_type, sensor_type);
	}

	else

	{
		map<int, Kinect *>::iterator it;
		for (it = this->devices.begin(); it != this->devices.end(); it++)
		{
			(*it).second->calibrate(calibration_type, sensor_type);
		}
	}
}

/** 
 * Method is used to handle export calibration data action
 * @return void
 */
void NITool::exportAction()
{	
	ofstream file;
	file.open(CFG_CALIBRATION_FILE);
	if (!file.is_open()) {
		cerr << "Error: Cannot export calibration parameters, file was not opened" << endl;
		exit(1);
	}

	file << "nccl_cam_calib_header=[" << endl;
	map<int, Kinect*>::iterator it;
	int i=0;
	for (i=0, it = this->devices.begin(); it != this->devices.end(); i++, it++)
	{
		Kinect* cam = (*it).second;
		cam->startStream(SENSOR_COLOR);
		cam->getSensorRGB()->loadIntrinsics((*it).second->getSensorRGB()->getStream());
		cam->getSensorRGB()->loadDistortionCoeffs((*it).second->getSensorRGB()->getStream());
		cam->getSensorRGB()->loadRotationVectors((*it).second->getSensorRGB()->getStream());
		cam->getSensorRGB()->loadTranslationVectors((*it).second->getSensorRGB()->getStream());
		cam->stopStream(SENSOR_COLOR);

		char buf[10];
		string cam_i = "\"cam" + string(_itoa(i, buf, 10));
		file << "\"" << cam->getCameraId() << "\" " << cam_i << "_distortion\" " 
			 << cam_i << "_K\" " << cam_i << "_R\" " << cam_i << "_t\";" << endl;
	}

	file << "]" << endl;

	for (i=0, it = this->devices.begin(); it != this->devices.end(); i++, it++)
	{
		Kinect* cam = (*it).second;
		//Save the camera K matrix
		file << "cam" << i << "_K=[" << endl;
		CvMat * K = cam->getSensorRGB()->getIntrinsics();
		file << scientific 
			 << "\t" << CV_MAT_ELEM(*K, float, 0, 0) << "  " << CV_MAT_ELEM(*K, float, 0, 1) << "  " << CV_MAT_ELEM(*K, float, 0, 2) << ";" << endl
			 << "\t" << CV_MAT_ELEM(*K, float, 1, 0) << "  " << CV_MAT_ELEM(*K, float, 1, 1) << "  " << CV_MAT_ELEM(*K, float, 1, 2) << ";" << endl
			 << "\t" << CV_MAT_ELEM(*K, float, 2, 0) << "  " << CV_MAT_ELEM(*K, float, 2, 1) << "  " << CV_MAT_ELEM(*K, float, 2, 2) << endl;
		file << "]" << endl;

		//Save the camera R matrix
		file << "cam" << i << "_R=[" << endl;
		CvMat * R_v = cam->getSensorRGB()->getRotationVectors();
		CvMat *R = cvCreateMat(3, 3, CV_32FC1);
		cvRodrigues2(R_v, R);
		cvTranspose(R, R);

		file << scientific 
			 << "\t" << CV_MAT_ELEM(*R, float, 0, 0) << "  " << CV_MAT_ELEM(*R, float, 0, 1) << "  " << CV_MAT_ELEM(*R, float, 0, 2) << ";" << endl
			 << "\t" << CV_MAT_ELEM(*R, float, 1, 0) << "  " << CV_MAT_ELEM(*R, float, 1, 1) << "  " << CV_MAT_ELEM(*R, float, 1, 2) << ";" << endl
			 << "\t" << CV_MAT_ELEM(*R, float, 2, 0) << "  " << CV_MAT_ELEM(*R, float, 2, 1) << "  " << CV_MAT_ELEM(*R, float, 2, 2) << endl;
		file << "]" << endl;
		
		//Save the camera distortion matrix
		file << "cam" << i << "_distortion=[" << endl;
		CvMat * d = cam->getSensorRGB()->getDistortionCoeffs();
		file << scientific << CV_MAT_ELEM(*d, float, 0, 0) << " " << CV_MAT_ELEM(*d, float, 1, 0) << " " << CV_MAT_ELEM(*d, float, 2, 0) << " " << CV_MAT_ELEM(*d, float, 3, 0) << " " << CV_MAT_ELEM(*d, float, 4, 0) << endl;
		file << "]" << endl;
		
		// Save the camera t vector
		file << "cam" << i << "_t=[" << endl;
		CvMat* t = cam->getSensorRGB()->getTranslationVectors();
		file << scientific << "\t" << -CV_MAT_ELEM(*t, float, 0, 0)<< "  " << -CV_MAT_ELEM(*t, float, 0, 1)<< "  " << -CV_MAT_ELEM(*t, float, 0, 2) << endl;
		file << "]" << endl; 
	}
	file.close();

	printf("Done...\n");
}

/** 
 * Method is used to handle undistortion action
 * @return void
 */
void NITool::undistortAction()
{
	map<int, Kinect*>::iterator it;
	Kinect* cam;
	for (it = this->devices.begin(); it != this->devices.end(); it++)
	{
		cam = (*it).second;
		cam->stopAllStreams();
		Stream *oStream = cam->startStream(SENSOR_COLOR);
		cam->getSensorRGB()->loadIntrinsics(oStream);
		cam->getSensorRGB()->loadDistortionCoeffs(oStream);		
		cam->getSensorRGB()->undistort(oStream->getVideoMode().getResolutionX(), oStream->getVideoMode().getResolutionY());		
	}

	string win_name = "NIToolbox - Camera %d";
	IplImage img_distorted;
	IplImage img_undistorted;
	bool bContinue = true;
	while(1)
	{
		int i=0;
		for (it = this->devices.begin(); it != this->devices.end(); it++)
		{
			vector<IplImage> vector;
			Kinect* cam = (*it).second;
			char buffer [50];
			sprintf(buffer, win_name.c_str(), cam->getCameraId());
			Stream *tmpStream = cam->getSensorRGB()->getStream();
			Mat frame_distorted = tmpStream->frameToMat(tmpStream->nextFrame());	
			Mat frame_undistorted = frame_distorted.clone();
			img_distorted = (IplImage) frame_distorted;
			vector.push_back(img_distorted);
			
			img_undistorted = (IplImage) frame_undistorted;
			IplImage *tmp = cvCloneImage( &img_undistorted );
			cvRemap( tmp, &img_undistorted, cam->getSensorRGB()->getUndistortMapX(), cam->getSensorRGB()->getUndistortMapY()); 
			vector.push_back(img_undistorted);
			
			bContinue = this->displayMultiFrame(buffer, vector, SENSOR_COLOR);
			i++;
			cvReleaseImage(&tmp);
		}

		if(!bContinue)
			break;
	}

	cvDestroyAllWindows();

	for (it = this->devices.begin(); it != this->devices.end(); it++)
	{
		Kinect* cam = (*it).second;
		cam->stopAllStreams();
	}

}

/** 
 * Method is used to run the NITool
 * @return void
 */
void NITool::run()
{
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
		throw runtime_error(OpenNI::getExtendedError());

	OpenNI::enumerateDevices(&deviceList);

	if(deviceList.getSize() < 1)
		throw runtime_error("There should be at least one connected camera! \n");

	printf("\nDetected devices: %d\n", deviceList.getSize());

	for(int i = 0; i < deviceList.getSize(); i++)
	{
		int id = i + 10000;		
		Kinect *k = new Kinect(id, deviceList[i].getUri());
		this->devices[id] = k;

		printf("Device %d - %s\n", id, deviceList[i].getUri());
	}

	char mode = ' ';
	while(mode != '0')
	{
		this->printHelp();	
		printf("\n>> ");
		cin>>mode;

		if(mode == '0')
		{
			this->finalize();
			break;
		}

		else

		if(mode == MODE_CAPTURE)
		{
			printf("OK!\n");
			this->captureAction();
		}

		else

		if(mode == MODE_CALIBRATION)
		{
			printf("OK!\n");
			this->calibrationAction();
		}
		
		else

		if(mode == MODE_UNDISORT)
		{
			printf("OK!\n");
			this->undistortAction();
		}

		else

		if(mode == MODE_EXPORT)
		{
			printf("OK!\n");
			this->exportAction();
		}

		else

		{
			printf("Invalid input: (%c/%c/%c/%c) is required.\n\n", MODE_CAPTURE, MODE_CALIBRATION, MODE_UNDISORT, MODE_EXPORT);
		}
	}
}

/** 
 * Method is used to finilize the NITool
 * @return void
 */
void NITool::finalize()
{
	for (map<int, Kinect *>::iterator it = this->devices.begin() ; it != this->devices.end(); it++)
	{
		(*it).second->stopAllStreams();
		(*it).second->close();		
	}

	OpenNI::shutdown();
}

/** 
 * Method is used to get the map containing the devices
 * @return map<int, Kinect *>	Pointer to the map containing the devices
 */
map<int, Kinect *> & NITool::getDevices()
{
	return devices;
}

/** 
 * Method is used to get the number of connected and initialized devices
 * @return int	The number of connected and initialized devices
 */
size_t NITool::getDeviceCount()
{
	return devices.size();
}

/** 
 * Utility function used to display multiple frames in a single window
 * @return bool
 */
bool NITool::displayMultiFrame(const char * title, vector<IplImage> frames, const SensorType & sensor_type)
{
	IplImage *img, *DispImage;
	DispImage = NULL;

    int max, size_w, size_h, i, m, n, x, y, w, h;
    float scale;

    size_t frames_count = frames.size();
	if (frames_count == 1) { w = h = 1; size_w = 640; size_h = 480; }
		else
	if (frames_count == 2) { w = 2; h = 1; size_w = 640; size_h = 480; }
		else 
	if (frames_count == 3 || frames_count == 4) { w = 2; h = 2; size_w = 320; size_h = 240; }
		else
	{ w = 4; h = 3; size_w = 45; size_h = 30; }

	switch(sensor_type) 
	{
		case SENSOR_COLOR:
			DispImage = cvCreateImage( cvSize(size_w*w, size_h*h), 8, 3 );
			break;
		case SENSOR_DEPTH:
			DispImage = cvCreateImage( cvSize(size_w*w, size_h*h), 16, 1);
			break;
		case SENSOR_IR:
			DispImage = cvCreateImage( cvSize(size_w*w, size_h*h), 8, 0);
			break;
	}

	vector<IplImage>::iterator it;
	for(it = frames.begin(), i=0, m = 0, n = 0; it != frames.end(); ++it)
	{
		img = &(*it);
		x = img->width;
        y = img->height;
        max = (x > y)? x: y; scale = (float) ( (float) max / size_w);
		cvSetImageROI(DispImage, cvRect(m, n, (int)( x/scale ), (int)( y/scale )));
        cvResize(img, DispImage);
        cvResetImageROI(DispImage);
		i++; m += size_w;
	}

    cvShowImage(title, DispImage);
	int c = cvWaitKey(CFG_CAPTURE_INTERVAL);
	cvReleaseImage(&DispImage);
	if(c == 'x')
		return false;

	return true;
}

/** 
 * Method is used to set the config param sensor type
 * @param SensorType	The type of the sensor to set
 * @return void
 */
void NITool::setSensorType(SensorType type)
{
	this->cfg_sensor_type = type;
}

/** 
 * Method is used to get the config param sensor type
 * @return SensorType
 */
SensorType NITool::getSensorType()
{
	return this->cfg_sensor_type;
}

/** 
 * Method is used to print the help menu
 * @return void
 */
void NITool::printHelp()
{
	cout<<"\n**********"<<endl
		<<"enter 1 to start data capturing"<<endl
		<<"enter 2 to perform calibration"<<endl
		<<"enter 3 to undistort"<<endl
		<<"enter 4 to export calibration file"<<endl		
		<<"enter 0 to exit"<<endl;
}

