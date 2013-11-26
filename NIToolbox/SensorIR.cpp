#include "stdafx.h"
#include "SensorIR.h"

/** 
 * Default constructor
 * @param int	The id of camera
 */
SensorIR::SensorIR(int camera_id)
{
	_camera_id = camera_id;
	ir = new Stream();
	depth =  new Stream();
}

/** 
 * Destructor
 */
SensorIR::~SensorIR()
{

}

/** 
 * Method is used to get the pointer to the IR stream object
 * @return Stream *
 */
Stream * SensorIR::getStreamIR()
{
	return this->ir;
}

/** 
 * Method is used to get the pointer to the depth stream object
 * @return Stream *
 */
Stream * SensorIR::getStreamDepth()
{
	return this->depth;
}