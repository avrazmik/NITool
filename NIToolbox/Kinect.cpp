#include "stdafx.h"
#include "Kinect.h"

/** 
 * Default constructor
 * @param int	The unique id of the camera
 * @param char *	The device uri, if not specified ANY_DEVICE will be open
 */
Kinect::Kinect(int id, const char* uri)
{
	Status rc;
	if(uri == NULL)
	{
		rc = this->open(ANY_DEVICE);
	}
	else
	{
		rc = this->open(uri);	
	}

	if (rc != STATUS_OK)
		throw runtime_error(OpenNI::getExtendedError());
	
	camera_id = id;

	ir = new SensorIR(camera_id);
	rgb = new SensorRGB(camera_id);
}

/**
 * Method is used to get the camera Id
 * @return int	Camera Id
 */
int Kinect::getCameraId()
{
	return camera_id;
}

/**
 * Method is used to get the pointer to the IR sensor object
 * @return SensorIR *	
 */
SensorIR * Kinect::getSensorIR()
{
	return this->ir;
}

/**
 * Method is used to get the pointer to the RGB sensor object
 * @return SensorIR *	
 */
SensorRGB * Kinect::getSensorRGB()
{
	return this->rgb;
}

/** 
 * Method is used to create and start the specified stream
 * @param SensorType	The type of the sensor to create and start
 * @return Stream *	The pointer of the stream
 */
Stream * Kinect::startStream(SensorType type)
{
	Status rc = STATUS_OK;

	rc = this->getStream(type)->create(*this, type);
	if (rc != STATUS_OK)
		throw runtime_error(OpenNI::getExtendedError());

	rc = this->getStream(type)->start();
	if (rc != STATUS_OK)
		throw runtime_error(OpenNI::getExtendedError());

	this->getStream(type)->_camera_id = camera_id;
	this->getStream(type)->mkdir();

	return this->getStream(type);
}

/** 
 * Method is used to stop and destroy the specified stream
 * @param SensorType	The type of the sensor to stop and destroy
 * @return void
 */
void Kinect::stopStream(SensorType type)
{
	this->getStream(type)->stop();
	this->getStream(type)->destroy();	
}

/** 
 * Method is used to get the specified stream
 * @param SensorType	The type of the sensor to start
 * @return Stream * The pointer of the stream
 */
Stream * Kinect::getStream(SensorType type)
{
	Stream *p = NULL;
	switch(type) {
		case SENSOR_COLOR:
			p = this->getSensorRGB()->getStream();
			break;
		case SENSOR_IR:
			p = this->getSensorIR()->getStreamIR();
			break;
		case SENSOR_DEPTH:
			p = this->getSensorIR()->getStreamDepth();
			break;
	}
	return p;
}

/** 
 * Method is used to stop all streams
 * @return void
 */
void Kinect::stopAllStreams()
{
	this->stopStream(SENSOR_COLOR);
	this->stopStream(SENSOR_DEPTH);
	this->stopStream(SENSOR_IR);
}

/**
 * Method is used to perform the calibration of Kinect camera
 * @param NI_CALIBRATION_TYPE	The type of the calibration
 * @param NI_SENSOR_TYPE	The type of sensor to calibrate
 * @return bool	Returns true in case of success, otherwise false
 */
bool Kinect::calibrate(char calib_type, char sensor_type)
{	
	bool id_calibrated = false;
	NI_CALIBRATION_TYPE ct = (NI_CALIBRATION_TYPE) calib_type;

	if(ct == NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC || ct == NI_CALIBRATION_TYPE_INTRINSIC || ct == NI_CALIBRATION_TYPE_EXTRINSIC)
	{
		if(sensor_type == NI_SENSOR_TYPE_RGB) 
		{
			SensorRGB *oSensorRGB = this->getSensorRGB();
			Stream *oStream = this->startStream(SENSOR_COLOR);

			if(ct == NI_CALIBRATION_TYPE_EXTRINSIC)
			{
				oSensorRGB->loadIntrinsics(oStream);
				oSensorRGB->loadDistortionCoeffs(oStream);	
			}

			id_calibrated = oSensorRGB->calibrate(ct, oStream);
			if(id_calibrated)
			{
				if(ct == NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC)
				{
					oSensorRGB->saveIntrinsics(oSensorRGB->getStream());
					oSensorRGB->saveDistortionCoeffs(oSensorRGB->getStream());	
					oSensorRGB->saveRotationVectors(oSensorRGB->getStream());
					oSensorRGB->saveTranslationVectors(oSensorRGB->getStream());
				}
				
				else

				if(ct == NI_CALIBRATION_TYPE_EXTRINSIC)
				{
					oSensorRGB->saveRotationVectors(oSensorRGB->getStream());
					oSensorRGB->saveTranslationVectors(oSensorRGB->getStream());
				}

				else

				if(ct == NI_CALIBRATION_TYPE_INTRINSIC)
				{
					oSensorRGB->saveIntrinsics(oSensorRGB->getStream());
					oSensorRGB->saveDistortionCoeffs(oSensorRGB->getStream());	
				}
			}

			oSensorRGB->getStream()->getPathBuffer().clear();
			this->stopStream(SENSOR_COLOR);
		}

		else

		if(sensor_type == NI_SENSOR_TYPE_IR)
		{
			SensorIR *oSensorIR = this->getSensorIR();
			Stream *oStream = this->startStream(SENSOR_IR);

			if(ct == NI_CALIBRATION_TYPE_EXTRINSIC)
			{
				oSensorIR->loadIntrinsics(oSensorIR->getStreamIR());
			}

			id_calibrated = oSensorIR->calibrate(ct, oStream);

			if(id_calibrated)
			{
				if(ct == NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC)
				{
					oSensorIR->saveIntrinsics(oSensorIR->getStreamIR());
					oSensorIR->saveDistortionCoeffs(oSensorIR->getStreamIR());	
					oSensorIR->saveRotationVectors(oSensorIR->getStreamIR());
					oSensorIR->saveTranslationVectors(oSensorIR->getStreamIR());
				}
				
				else

				if(ct == NI_CALIBRATION_TYPE_EXTRINSIC)
				{
					oSensorIR->saveRotationVectors(oSensorIR->getStreamIR());
					oSensorIR->saveTranslationVectors(oSensorIR->getStreamIR());
				}

				else

				if(ct == NI_CALIBRATION_TYPE_INTRINSIC)
				{
					oSensorIR->saveIntrinsics(oSensorIR->getStreamIR());
					oSensorIR->saveDistortionCoeffs(oSensorIR->getStreamIR());		
				}
			}

			oSensorIR->getStreamIR()->getPathBuffer().clear();
			this->stopStream(SENSOR_IR);
		}
	}

	else

	if(ct == NI_CALIBRATION_TYPE_DEPTH)
	{
		id_calibrated = this->calibrateDepth();
	}

	else

	if(ct == NI_CALIBRATION_TYPE_PAIR)
	{
		id_calibrated = this->calibratePair();
	}

	return id_calibrated;
}

/**
 * Method is used to perform the calibration of IR and RGB sensor pair
 * @return bool	Returns true in case of success, otherwise false
 */
bool Kinect::calibratePair()
{
	Mat mat_images[2];
	int number_of_corners = CFG_BOARD_W * CFG_BOARD_H;
	CvSize board_sz = cvSize(CFG_BOARD_W, CFG_BOARD_H);
	CvPoint2D32f * ir_corners = new CvPoint2D32f[number_of_corners];
	CvPoint2D32f * rgb_corners = new CvPoint2D32f[number_of_corners];
	int corner_count;

	int found_ir = 0;
	int found_rgb = 0;
	int step = 0, successes = 0;
	int total_view_per_cam = 1;

	CvMat* ir_image_points	= cvCreateMat(number_of_corners * total_view_per_cam, 2, CV_32FC1);
	CvMat* rgb_image_points	= cvCreateMat(number_of_corners * total_view_per_cam, 2, CV_32FC1);
	CvMat* object_points = cvCreateMat(number_of_corners * total_view_per_cam, 3, CV_32FC1);
	CvMat* point_counts = cvCreateMat(total_view_per_cam, 1, CV_32SC1);

	SensorIR *oSensorIR = this->getSensorIR(); 
	SensorRGB *oSensorRGB = this->getSensorRGB();
	Stream *irStream, *rgbStream;
	
	while(1)
	{
		irStream = this->startStream(SENSOR_IR);
		oSensorIR->loadIntrinsics(irStream);
		oSensorIR->loadDistortionCoeffs(irStream);

		mat_images[0] = irStream->frameToMat(irStream->nextFrame());
		//irStream->saveFrame(mat_images[0]);
		this->stopStream(SENSOR_IR);
		rgbStream = this->startStream(SENSOR_COLOR);

		oSensorRGB->loadIntrinsics(rgbStream);
		oSensorRGB->loadDistortionCoeffs(rgbStream);

		mat_images[1] = rgbStream->frameToMat(rgbStream->nextFrame());
		//rgbStream->saveFrame(mat_images[1]);
		
		//blurring the IR image to avoid the noise
		medianBlur(mat_images[0], mat_images[0], 5);

		IplImage *tmp_ir = &((IplImage) mat_images[0]);
		found_ir = cvFindChessboardCorners(tmp_ir, board_sz, ir_corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		if(found_ir && corner_count == number_of_corners) 
		{
			cvFindCornerSubPix(tmp_ir, ir_corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));
			cvDrawChessboardCorners(tmp_ir, board_sz, ir_corners, corner_count, found_ir);						
		}
		
		IplImage *tmp_rgb = &((IplImage) mat_images[1]);
		found_rgb = cvFindChessboardCorners(tmp_rgb, board_sz, rgb_corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		if(found_rgb && corner_count == number_of_corners) 
		{
			IplImage *gray_image = cvCreateImage(cvSize(rgbStream->getVideoMode().getResolutionX(), rgbStream->getVideoMode().getResolutionY()), 8, 1);
			
			cvCvtColor( tmp_rgb, gray_image, CV_BGR2GRAY);
			cvFindCornerSubPix(gray_image, rgb_corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));					
			cvDrawChessboardCorners(tmp_rgb, board_sz, rgb_corners, corner_count, found_rgb);
		}
		
		this->stopStream(SENSOR_COLOR);

		if(found_ir && found_rgb)
		{
			step = successes * number_of_corners;
			
			for(int i=step, j=0; j < number_of_corners; ++i, ++j)
			{
				CV_MAT_ELEM(*rgb_image_points, float, i, 0) = rgb_corners[j].x;
				CV_MAT_ELEM(*rgb_image_points, float, i, 1) = rgb_corners[j].y;

				CV_MAT_ELEM(*ir_image_points, float, i, 0) = ir_corners[j].x;
				CV_MAT_ELEM(*ir_image_points, float, i, 1) = ir_corners[j].y;

				CV_MAT_ELEM(*object_points, float, i, 0) = (float) (j / CFG_BOARD_W);
				CV_MAT_ELEM(*object_points, float, i, 1) = (float) (j % CFG_BOARD_W);
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}

			CV_MAT_ELEM(*point_counts, int, successes, 0) = number_of_corners;
			successes++;
		}		

		cvShowImage("IR", tmp_ir);
		cvShowImage("RGB", tmp_rgb);
		cvWaitKey(2000);

		if(successes == total_view_per_cam)
		{			
			cout<<'\a';
			break;
		}
		
	}

	cvDestroyAllWindows();

	CvMat * ir_intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat * ir_distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);

	CvMat * rgb_intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat * rgb_distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);

	CvMat* rotation_matrix = cvCreateMat(3, 3, CV_32FC1);
	CvMat* translation_vector = cvCreateMat(3, 1, CV_32FC1);
	
	ir_intrinsic_matrix = this->getSensorIR()->getIntrinsics();
	rgb_intrinsic_matrix = this->getSensorRGB()->getIntrinsics();

	ir_distortion_coeffs = this->getSensorIR()->getDistortionCoeffs();
	rgb_distortion_coeffs = this->getSensorRGB()->getDistortionCoeffs();

	//cvZero(ir_distortion_coeffs);
	//cvZero(rgb_distortion_coeffs);
	 
	double rep_error = cvStereoCalibrate
	(
		object_points, rgb_image_points, ir_image_points, point_counts, 
		rgb_intrinsic_matrix, rgb_distortion_coeffs,
		ir_intrinsic_matrix, ir_distortion_coeffs,		
		cvSize(640, 480), rotation_matrix, translation_vector, 0, 0, cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5), CV_CALIB_FIX_INTRINSIC
	); 

	cout<<"\nTranslation vector"<<endl;
	for(int i=0; i < translation_vector->rows; i++)
	{
		cout<<CV_MAT_ELEM(*translation_vector, float, i, 0)<<" ";
	}

	CvMat* rotation_vector = cvCreateMat(1, 3, CV_32FC1);
	cvRodrigues2(rotation_matrix, rotation_vector);

	cout<<"\n\nRotation vector"<<endl;
	for(int i=0; i < rotation_vector->cols; i++)
	{
		cout<<CV_MAT_ELEM(*rotation_vector, float, 0, i)<<" ";
	}

	cout<<"\n\nRep. Error "<<rep_error<<endl;
	cout<<endl;

	this->stopAllStreams();
	
	return true;
}

/**
 * Method is used to perform depth calibration
 * uses the object of OptiTrack to initate the connection with the tracking system
 * @return bool	Returns true in case of success, otherwise false
 */
bool Kinect::calibrateDepth()
{	
	//pthread_t thread;
	//pthread_create(&thread, NULL, glut_vis, (void*) this);
	
	Stream *irStream = this->startStream(SENSOR_IR);
	Stream *depthStream = this->startStream(SENSOR_DEPTH);
	
	int default_depth = 2500;
	
	printf("Creating a lookup table of a size %dx%dx%d...\n", depthStream->getVideoMode().getResolutionX(), depthStream->getVideoMode().getResolutionY(), default_depth);
	Lookup *lookup = this->getSensorIR()->createLookup(depthStream->getVideoMode().getResolutionX(), depthStream->getVideoMode().getResolutionY(), default_depth);
	this->getSensorIR()->loadLookup(this->getSensorIR()->getStreamDepth());
	
	/*
	OptiTrack *oStream = new OptiTrack("", CFG_TRACKING_SERVER_IP);
	oStream->SetVerbosityLevel(Verbosity_None);
	oStream->SetDataCallback(this->DataHandler, this);
	oStream->Connect();
	
	int retCode;
	void* response;
	int nBytes;
	retCode = oStream->SendMessageAndWait("UnitsToMillimeters", &response, &nBytes);
	if (retCode == ErrorCode_OK) 
		OptiTrack::setUnitConvertion(*(float*)response);
	
	CvPoint3D32f * positions = new CvPoint3D32f[CFG_BOARD_W * CFG_BOARD_H];
	int number_of_corners = CFG_BOARD_W * CFG_BOARD_H;
	CvSize board_sz = cvSize(CFG_BOARD_W, CFG_BOARD_H);
	CvPoint2D32f * corners = new CvPoint2D32f[number_of_corners];
	int corner_count;

	while(1)
	{
		bool is_valid = true;
		if(OptiTrack::current_frame == NULL)
			is_valid = false;

		if(OptiTrack::current_frame->nRigidBodies < 1)
			is_valid = false;

		if(OptiTrack::current_frame->RigidBodies[0].x == 0 || OptiTrack::current_frame->RigidBodies[0].y == 0 || OptiTrack::current_frame->RigidBodies[0].z == 0)
			is_valid = false;

		IplImage *gray_image = cvCreateImage(cvSize(irStream->getVideoMode().getResolutionX(), irStream->getVideoMode().getResolutionY()), 8, 1);

		Mat ir_img = irStream->frameToMat(irStream->nextFrame());
		IplImage img = ((IplImage) ir_img);
		Mat blurred;
		medianBlur(ir_img, blurred, 5);
		IplImage ir_blurred = ((IplImage) blurred);
		int found = cvFindChessboardCorners(&ir_blurred, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS);	
		cvFindCornerSubPix( &ir_blurred, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));			
		VideoFrameRef ref = depthStream->nextFrame();
		
		Mat depth_mat = depthStream->frameToMat(ref);
		IplImage depth_img = (IplImage) depth_mat;
		
		if(found && corner_count == number_of_corners && is_valid)
		{
			printf("----------------------\n");
			this->getSensorIR()->detected_targets++;
			this->getSensorIR()->findChessboardCornerPositions(OptiTrack::current_frame, positions);

			DepthPixel* pDepth = (DepthPixel*) ref.getData();
			int avg_error = 0;
			int grp_avg_error = 0;

			for(int i=0; i<corner_count; i++)
			{
				this->getSensorIR()->subtractOffet(&corners[i]);
				
				int xx = (int) corners[i].x;
				int yy = (int) corners[i].y;

				int depth_index = ref.getWidth() * yy + xx;
				
				int distance_row = (i/CFG_BOARD_W + 1);
				int distance_index = (distance_row * CFG_BOARD_W - (i) % CFG_BOARD_W) - 1;
				float distance = sqrt(
					(positions[distance_index].x * positions[distance_index].x) +
					(positions[distance_index].y * positions[distance_index].y) +
					(positions[distance_index].z * positions[distance_index].z)
				);

				printf("Depth value: %d, Estimated distance: %d, Offset: %d\n", pDepth[depth_index], (int) distance, (int) distance - pDepth[depth_index]);	
				avg_error += abs((int) distance - pDepth[depth_index]);
				grp_avg_error += abs((int) distance - pDepth[depth_index]);
				if((i+1) % 8 == 0)
				{
					printf("\nGr. Avg Error:%d\n\n", grp_avg_error / 8);
					grp_avg_error = 0;
				}
				
				if((int) pDepth[depth_index] < this->getSensorIR()->getLookup()->getDepth() 
					&& xx < this->getSensorIR()->getLookup()->getWidth() && yy < this->getSensorIR()->getLookup()->getHeight())
					this->getSensorIR()->getLookup()->At(xx, yy, ((int) pDepth[depth_index]))->value((int) distance);	
				
			}

			printf("Avg Error:%d\n", avg_error / corner_count);
		}

		cvDrawChessboardCorners(&depth_img, board_sz, corners, corner_count, found);
		cvShowImage("Depth Stream", &depth_img);
		int c = cvWaitKey(500);
		if(c == 'x')
			break;

		cvReleaseImage(&gray_image);
	}
	
	this->stopAllStreams();
	
	oStream->Disconnect();
	cvDestroyAllWindows();
	*/

	char dir[100];		
	sprintf(dir, CFG_STORAGE, this->camera_id);
	strcat(dir, this->getSensorIR()->getStreamDepth()->getStoragePrefix());
	
	LookupGL glViz;
	LookupGL::lookupPath = dir;
	glViz.minify(lookup);
	glViz.visualize();
	
	return true;
}

/**
 * A static method is used to visualize the depth lookup table by using threads
 * @param void *	A user specified argument
 * @return void
 */
void * Kinect::glut_vis(void *ptr)
{
	Kinect * oKinect = static_cast<Kinect*>(ptr);
	printf("Creating the table...\n");
	Lookup *lookup = oKinect->getSensorIR()->createLookup(320, 240, 3000);
	LookupGL glut;
	glut.setLookup(lookup);
	glut.visualize();
	return ptr;
}


/**
 * Method is used by thread to handle glut loop
 * @param void * Input parameter to pass in
 * @return void *
 */
void *Kinect::glut_loop( void *ptr )
{
	return ptr;
}

/**
 * Method-callback datahandler. Is triggered each time when a new frame from tracking system is available
 * @param sFrameOfMocapData	The actual data returned by the tracking server. Contains information about trackables.
 * @param void* The instance of this
 */
void __cdecl Kinect::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	OptiTrack::current_frame = data;
}