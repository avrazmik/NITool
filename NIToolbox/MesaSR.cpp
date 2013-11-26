#include "stdafx.h"
#include "MesaSR.h"

/** 
 * Default constructor
 * @param CMesaDevice *	A pointer to the instance of Mesa device
 */
MesaSR::MesaSR(CMesaDevice *srCam)
{
	this->srCam = srCam;	
	SR_SetIntegrationTime(srCam, 150);
	cols = SR_GetCols(srCam);
	rows = SR_GetRows(srCam);
	SR_SetMode(srCam, AM_CONF_MAP | AM_CONV_GRAY);	
}

/** 
 * Method is used to get the reference to the CMesaDevice instance
 * @return CMesaDevice *
 */
CMesaDevice* MesaSR::getDevice()
{
	return this->srCam;
}

/**
 * Method is used to perform depth calibration
 * uses the object of OptiTrack to initate the connection with the tracking system
 * @return bool	Returns true in case of success, otherwise false
 */
bool MesaSR::calibrateDepth()
{
	pthread_t thread;
	pthread_create(&thread, NULL, glut_vis, (void*) this);
	OptiTrack *oStream = new OptiTrack("", CFG_TRACKING_SERVER_IP);
	oStream->SetVerbosityLevel(Verbosity_None);
	oStream->SetDataCallback(this->DataHandler, this);
	printf("Starting acquire tracking data...\n");
	oStream->Connect();
	
	CvPoint3D32f * positions = new CvPoint3D32f[CFG_BOARD_W * CFG_BOARD_H];
	CMesaDevice*srCam = this->getDevice();
	int cols = SR_GetCols(srCam);
	int rows = SR_GetRows(srCam);
	int cfg_scale = 3;
	int number_of_corners = CFG_BOARD_W * CFG_BOARD_H;
	CvSize board_sz = cvSize(CFG_BOARD_W, CFG_BOARD_H);
	CvPoint2D32f * corners = new CvPoint2D32f[number_of_corners];
	int corner_count;

	while(1)
	{
		if(OptiTrack::current_frame == NULL)
			continue;

		if(OptiTrack::current_frame->nRigidBodies != 1)
			continue;
		
		if(this->getLookup() == NULL)
			continue;
			 
		SR_Acquire(srCam);
		unsigned short *distance_data = (unsigned short*) SR_GetImage(srCam, 0);
		unsigned short *amplutude_data = (unsigned short*) SR_GetImage(srCam, 1);
		
		Mat *frame1 = new Mat(rows, cols, CV_16UC1, distance_data);
		Mat *frame2 = new Mat(rows, cols, CV_16UC1, amplutude_data);
		resize(*frame2, *frame2, cvSize(cols * cfg_scale, rows * cfg_scale));

		IplImage depth_img = (IplImage) * frame1;
		IplImage ir_img = (IplImage) * frame2;
		IplImage *gray_image = cvCreateImage(cvSize(cols * cfg_scale, rows * cfg_scale), 8, 1);

		cvConvertScale(&ir_img, gray_image, 1.0/256, 0); 
		int found = cvFindChessboardCorners(gray_image, board_sz, corners, &corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS);	
		
		if(found && corner_count == CFG_BOARD_W * CFG_BOARD_H)
		{
			cvFindCornerSubPix(gray_image, corners, corner_count, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ));			
			cvDrawChessboardCorners(&ir_img, board_sz, corners, corner_count, found);
			
			this->findChessboardCornerPositions(OptiTrack::current_frame, positions);
			
			for(int i=0; i < corner_count; i++)
			{
				int xx = (int) corners[i].x / cfg_scale;
				int yy = (int) corners[i].y / cfg_scale;

				Point2f a(xx, yy);
				circle(*frame1, a, 1, Scalar(255, 255, 255));

				int distance_row = (i/CFG_BOARD_W + 1);
				int distance_index = (distance_row * CFG_BOARD_W - (i) % CFG_BOARD_W) - 1;

				float distance = sqrt
				(
					(positions[distance_index].x * positions[distance_index].x) +
					(positions[distance_index].y * positions[distance_index].y) +
					(positions[distance_index].z * positions[distance_index].z)
				);

				int depth_index = cols * yy + xx;
					
				cout<<distance<<" - "<<(distance_data[depth_index] / 10)<<endl;
			
				if((distance_data[depth_index] / 10) < this->getLookup()->getDepth())
					this->getLookup()->At(xx, yy, (distance_data[depth_index] / 10))->value((int) distance);				
			}

			this->detected_targets++;
		}

		cvShowImage("MESA SR4000", &((IplImage) * frame1));
		cvWaitKey(30);
		
		frame1->release();
		frame2->release();
		cvReleaseImage(&gray_image);
	}
	
	oStream->Disconnect();
	pthread_join(thread, NULL);
	
	return true;
}

/**
 * A static method is used to visualize the depth lookup table by using threads
 * @param void *	A user specified argument
 * @return void
 */
void * MesaSR::glut_vis(void *ptr)
{
	MesaSR * oMesaSR = static_cast<MesaSR*>(ptr);
	printf("Creating the table...\n");
	Lookup *lookup = oMesaSR->createLookup(oMesaSR->cols, oMesaSR->rows, 3000);
	LookupGL glut;
	glut.setLookup(lookup);
	glut.visualize();
	return ptr;
}

/**
 * A static callback function used to acquire data frames streammed by the tracking system
 * @param sFrameOfMocapData *	The data supplied by the tracking system
 * @param void*		A user specified argument
 * @return void
 */
void __cdecl MesaSR::DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	OptiTrack::current_frame = data;
}