#pragma once
#include "ColorModel.h"
#include "Stream.h"

class SensorRGB: public ColorModel
{
public:
	SensorRGB(int);
	Stream * getStream();
	int _camera_id;	
private:	
	Stream *rgb;
};