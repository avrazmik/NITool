#pragma once
#include "ColorModel.h"
#include "DepthModel.h"
#include "Stream.h"

class SensorIR: public ColorModel, public DepthModel
{
public:
	SensorIR(int);
	~SensorIR(void);
	Stream * getStreamIR();
	Stream * getStreamDepth();
	int _camera_id;
private:
	Stream *ir;
	Stream *depth;
};