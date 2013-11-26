#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Lookup.h"
#include "OptiTrack.h"
#include "Stream.h"

class DepthModel
{
public:
	DepthModel(void);
	~DepthModel(void);
	void subtractOffet(CvPoint2D32f *, int = 5, int = 4);
	Lookup * createLookup(int, int, int);
	Lookup * getLookup();
	int detected_targets;
	void saveLookup(Stream *);
	void loadLookup(Stream *);
	void findChessboardCornerPositions(sFrameOfMocapData* , CvPoint3D32f *positions);
private: 
	Lookup *lookup;
};

