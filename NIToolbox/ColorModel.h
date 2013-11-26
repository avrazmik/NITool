#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "inc.h"
#include "Stream.h"

using namespace cv;

class ColorModel
{
public:
	ColorModel(void);
	~ColorModel(void);

	bool calibrate(NI_CALIBRATION_TYPE, Stream *, bool = true);
	IplImage * getUndistortMapX();
	IplImage * getUndistortMapY();

	void saveIntrinsics(Stream *);
	CvMat * loadIntrinsics(Stream *);
	CvMat* getIntrinsics();

	void saveDistortionCoeffs(Stream *);
	CvMat* loadDistortionCoeffs(Stream *);
	CvMat* getDistortionCoeffs();

	void saveRotationVectors(Stream *);
	CvMat * loadRotationVectors(Stream *);
	CvMat* getRotationVectors();

	void saveTranslationVectors(Stream *);
	CvMat * loadTranslationVectors(Stream *);
	CvMat* getTranslationVectors();

	void cameraPos(CvMat *);	
	void undistort(int, int);
private:
	CvMat* intrinsic_matrix;
	CvMat* distortion_coeffs;
	CvMat* rotation_vectors;
	CvMat* translation_vectors;
	IplImage* mapx;
	IplImage* mapy;
};

