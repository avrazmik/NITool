#include "stdafx.h"
#include "SensorRGB.h"

/** 
 * Default constructor
 * @param int	The id of camera
 */
SensorRGB::SensorRGB(int camera_id)
{
	_camera_id = camera_id;
	rgb = new Stream();
}


/** 
 * Method is used to get the pointer to the RGB stream object
 * @return Stream *
 */
Stream * SensorRGB::getStream()
{
	return this->rgb;
}