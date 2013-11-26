#include "stdafx.h"
#include "ColorModel.h"

/** 
 * Default constructor
 */
ColorModel::ColorModel()
{
	intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);
	distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);
	rotation_vectors = cvCreateMat(1, 3, CV_32FC1);
	translation_vectors = cvCreateMat(1, 3, CV_32FC1);
}

/** 
 * Destructor
 */
ColorModel::~ColorModel()
{

}

/**
 * Method is used for perform calibration according OpenCV routines
 * @param	NI_CALIBRATION_TYPE	The type of calibration
 * @param	vector<IplImage>*	Pointer to the image set that is used for calibration
 * @param	bool	If passed true the calibration progress will be shown, otherwise no
 * @return	bool
 */
bool ColorModel::calibrate(NI_CALIBRATION_TYPE calib_type, Stream *oStream, bool b_visualize)
{
	vector<IplImage> set;
	int nFrames = 0;
	bool b_online = false;
	if(!oStream->loadFilePaths() || calib_type == NI_CALIBRATION_TYPE_EXTRINSIC)
	{
		b_online = true;
	}
	
	if(!b_online)
	{
		vector<string> path_buffer = oStream->getPathBuffer();
		IplImage * tmp_img = NULL;
		vector<string>::iterator it;
		for(it = path_buffer.begin(); it != path_buffer.end(); ++it)
		{
			tmp_img = cvLoadImage((*it).c_str());
			set.push_back(*tmp_img);
		}

		nFrames = set.size();
	}
	
	else

	{
		if(calib_type == NI_CALIBRATION_TYPE_EXTRINSIC)
			nFrames = 1;
		else
			nFrames = CFG_INT_DEFAULT_CAPTURE_COUNT;
	}

	int number_of_corners = CFG_BOARD_W * CFG_BOARD_H;
	CvSize board_sz = cvSize(CFG_BOARD_W, CFG_BOARD_H);
	CvPoint2D32f * corners = new CvPoint2D32f[number_of_corners];
	int corner_count;

	CvMat* image_points	= cvCreateMat(nFrames * number_of_corners, 2, CV_32FC1);
	CvMat* object_points = cvCreateMat(nFrames * number_of_corners, 3, CV_32FC1);
	CvMat* point_counts = cvCreateMat(nFrames, 1, CV_32SC1);

	IplImage *gray_image = cvCreateImage(cvSize(oStream->getVideoMode().getResolutionX(), oStream->getVideoMode().getResolutionY()), 8, 1);

	int step = 0, successes = 0;	
	
	if(b_visualize)
	{
		printf("\n");
		printf("***** Calibration \n");	
		printf("Starting...\n");
	}
	
	IplImage * image;
	int i = 0;
	Mat tmp;
	while(1)
	{
		if(b_online)
		{
			tmp = oStream->frameToMat(oStream->nextFrame());
			image = &((IplImage) tmp);
			if(successes == nFrames)
				break;
		}

		else
		
		{
			if(i == nFrames)
				break;

			image = &set[i];
			i++;
		}

		int found = cvFindChessboardCorners(image, board_sz, corners, &corner_count,  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);		
		if(oStream->getSensorInfo().getSensorType() == SENSOR_IR)
		{
			medianBlur(tmp, tmp, 5);
		}
		
		if(image->nChannels == 3)
		{
			cvCvtColor(image, gray_image, CV_BGR2GRAY);
			cvFindCornerSubPix(gray_image, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));			
		}
		else
		{
			cvFindCornerSubPix(image, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));			
		}
		 
		if(b_visualize)
		{
			cvDrawChessboardCorners(image, board_sz, corners, corner_count, found);
			cvShowImage("Calibration", image);
			int waitPeriod = CFG_CAPTURE_INTERVAL/2;
			if(b_online && calib_type == NI_CALIBRATION_TYPE_EXTRINSIC)
				waitPeriod = CFG_CAPTURE_INTERVAL * 2;
			else
			if(b_online)
				waitPeriod = CFG_CAPTURE_INTERVAL;

			cvWaitKey(waitPeriod);
		}

		if(found && corner_count == number_of_corners)
		{
			cout<<'\a';
			step = successes * number_of_corners;
			for(int i=step, j=0; j < number_of_corners; ++i, ++j)
			{
				CV_MAT_ELEM(*image_points, float, i, 0) = corners[j].x;
				CV_MAT_ELEM(*image_points, float, i, 1) = corners[j].y;
				CV_MAT_ELEM(*object_points, float, i, 0) = (float) (j / CFG_BOARD_W);
				CV_MAT_ELEM(*object_points, float, i, 1) = (float) (j % CFG_BOARD_W);
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}

			CV_MAT_ELEM(*point_counts, int, successes, 0) = number_of_corners;
			successes++;
		}

	}

	if(b_visualize) 
		cvDestroyWindow("Calibration");

	bool is_calibrated = false;
	if(successes > 0)
	{
		CvMat* image_points_2 = cvCreateMat(successes * number_of_corners, 2, CV_32FC1);
		CvMat* point_counts_2 = cvCreateMat(successes, 1, CV_32SC1 );
		CvMat* object_points_2 = cvCreateMat(successes * number_of_corners, 3, CV_32FC1);
		
		for(int i = 0; i < successes * number_of_corners; ++i)
		{
			CV_MAT_ELEM(*image_points_2, float, i, 0) = CV_MAT_ELEM(*image_points, float, i, 0);
			CV_MAT_ELEM(*image_points_2, float, i, 1) = CV_MAT_ELEM(*image_points, float, i, 1);
			CV_MAT_ELEM(*object_points_2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
			CV_MAT_ELEM(*object_points_2, float, i, 1) = CV_MAT_ELEM(*object_points, float, i, 1);
			CV_MAT_ELEM(*object_points_2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);			
		}
		
		for(int i=0; i < successes; ++i)
		{
			CV_MAT_ELEM( *point_counts_2, int, i, 0 ) = CV_MAT_ELEM(*point_counts, int, i, 0);
		}

		cvReleaseMat(&object_points);
		cvReleaseMat(&image_points);
		cvReleaseMat(&point_counts);
		
		double reprojection_errors = 0; 

		if(calib_type == NI_CALIBRATION_TYPE_EXTRINSIC)
		{
			CV_MAT_ELEM(*translation_vectors, float, 0, 0) = 0.0;
			CV_MAT_ELEM(*translation_vectors, float, 0, 1) = 0.0;
			CV_MAT_ELEM(*translation_vectors, float, 0, 2) = 0.0;

			cvFindExtrinsicCameraParams2(object_points_2, image_points_2, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);			
		}

		else

		{
			CvMat *tmp_rotation_vectors = cvCreateMat(successes, 3, CV_32FC1);
			CvMat *tmp_translation_vectors = cvCreateMat(successes, 3, CV_32FC1);

			CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) = 1.0;
			CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1) = 1.0;	

			reprojection_errors = cvCalibrateCamera2(object_points_2, image_points_2, point_counts_2, cvSize(oStream->getVideoMode().getResolutionX(), oStream->getVideoMode().getResolutionY()), intrinsic_matrix, distortion_coeffs,  tmp_rotation_vectors, tmp_translation_vectors, 0 ); 	
			printf("\nThe reprojection error was %f\n", reprojection_errors);

			if(calib_type == NI_CALIBRATION_TYPE_INTRINSIC_EXTRINSIC)
			{
				rotation_vectors = tmp_rotation_vectors;
				translation_vectors = tmp_translation_vectors;
			}
		}

		is_calibrated = true;
	}
	
	else

	{
		cerr<<"No target detected.\n"<<endl;
	}

	return is_calibrated;
}

/**
 * Method is used to get the undistortion map x
 * @return	IplImage *
 */
IplImage * ColorModel::getUndistortMapX()
{
	return this->mapx;
}

/**
 * Method is used to get the undistortion map y
 * @return	IplImage *
 */
IplImage * ColorModel::getUndistortMapY()
{
	return this->mapy;
}

/**
 * Method is used to load the intrinsic_matrix from xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return	CvMat *
 */
CvMat * ColorModel::loadIntrinsics(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "intrinsics.xml");
	intrinsic_matrix = (CvMat*)cvLoad(dir); 
	if(intrinsic_matrix == NULL)
		throw runtime_error("The intrinsic matrix cannot be loaded. Exiting. \n");

	return intrinsic_matrix;
}


/**
 * Method is used to save the intrinsic_matrix into xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return void
 */
void ColorModel::saveIntrinsics(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "intrinsics.xml");
	cvSave(dir, intrinsic_matrix);
}

/**
 * Method is used to get the intrinsic_matrix
 * @return	CvMat *
 */
CvMat* ColorModel::getIntrinsics()
{
	return intrinsic_matrix;
}

/**
 * Method is used to load the distortion_coeffs from xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return	CvMat *
 */
CvMat * ColorModel::loadDistortionCoeffs(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "distortion_coefficents.xml");
	distortion_coeffs = (CvMat*)cvLoad(dir);
	if(distortion_coeffs == NULL)
		throw runtime_error("The distortion coefficients cannot be loaded. Exiting. \n");
	return distortion_coeffs;
}

/**
 * Method is used to save the distortion_coeffs into xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return void
 */
void ColorModel::saveDistortionCoeffs(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "distortion_coefficents.xml");
	cvSave(dir, distortion_coeffs);
}

/**
 * Method is used to get the distortion_coeffs
 * @return	CvMat *
 */
CvMat* ColorModel::getDistortionCoeffs()
{
	return distortion_coeffs;
}

/**
 * Method is used to save the translation_vectors into XML file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return void
 */
void ColorModel::saveRotationVectors(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "rotation_vectors.xml");
	cvSave(dir, rotation_vectors);
}

/**
 * Method is used to load the rotation_vectors from xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 * @return void
 */
CvMat * ColorModel::loadRotationVectors(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "rotation_vectors.xml");
	rotation_vectors = (CvMat*)cvLoad(dir);
	if(rotation_vectors == NULL)
		throw runtime_error("The rotation vectors cannot be loaded. Exiting. \n");
	return rotation_vectors;
}

/**
 * Method is used to get the rotation_vectors
 * @return	CvMat *
 */
CvMat* ColorModel::getRotationVectors()
{
	return rotation_vectors;
}

/**
 * Method is used to save the translation_vectors into XML file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 */
void ColorModel::saveTranslationVectors(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "translation_vectors.xml");
	cvSave(dir, translation_vectors);
}

/**
 * Method is used to load the translation_vectors from xml file
 * @param	Stream *	Pointer to the stream. Used to find the path of the folder
 */
CvMat * ColorModel::loadTranslationVectors(Stream *p)
{
	char dir[100];		
	sprintf(dir, CFG_STORAGE, p->_camera_id);
	strcat(dir, p->getStoragePrefix());
	strcat(dir, "translation_vectors.xml");
	translation_vectors = (CvMat*)cvLoad(dir);
	if(rotation_vectors == NULL)
		throw runtime_error("The translation vectors cannot be loaded. Exiting. \n");
	return translation_vectors;
}

/**
 * Method is used to get the translation_vectors
 * @return	CvMat *
 */
CvMat* ColorModel::getTranslationVectors()
{
	return translation_vectors;
}

/**
 * Method is used to calculate the position of the sensor based on the intrinsic and extrinsic parameters 
 * uses the following eq. C = -inv(R)*T to find the position w.r.t world coordinate system
 * @param CvMat *	The 1x3 vector which will be filled with the real position values
 * @return void
 */
void ColorModel::cameraPos(CvMat *camera_position)
{
	for(int j = 0; j < camera_position->cols; j++)
		CV_MAT_ELEM(*camera_position, float, 0, j) = 0.0;

	CvMat* camera_translation = this->getTranslationVectors();
	CvMat* camera_rotation = cvCreateMat(3, 3, CV_32FC1);
	CvMat* camera_rotation_t = cvCreateMat(3, 3, CV_32FC1);
	cvRodrigues2(this->getRotationVectors(), camera_rotation);
	cvTranspose(camera_rotation, camera_rotation_t);
	for(int i = 0; i < camera_rotation_t->cols; i++)
	{
		for(int j = 0; j < camera_rotation_t->rows; j++)
		{
			CV_MAT_ELEM(*camera_position, float, 0, i) += (-CV_MAT_ELEM(*camera_rotation_t, float, i, j)) * CV_MAT_ELEM(*camera_translation, float, 0, j);
		}
	}
}

/**
 * Method is used to initialize undistortion map 
 * @return void
 */
void ColorModel::undistort(int resx, int resy)
{
	mapx = cvCreateImage( cvSize(resx, resy), IPL_DEPTH_32F, 1 );
	mapy = cvCreateImage( cvSize(resx, resy), IPL_DEPTH_32F, 1 );
	cvInitUndistortMap(intrinsic_matrix, distortion_coeffs, mapx, mapy );
}