#include "stdafx.h"
#include "LookupItem.h"


/** 
 * Default constructor 
 */
LookupItem::LookupItem()
{
	this->init();
}

/** 
 * Default destructor
 */
LookupItem::~LookupItem()
{
}

/** 
 * Method is used to set the distance value to the lookup item
 * @param unsigned short int	The observed distance to be added
 */
void LookupItem::value(unsigned short int distance)
{
	mm_distance = distance;
}

/** 
 * Method is used to get the value of field distances
 * @return unsigned short int
 */
unsigned short int LookupItem::value()
{
	return mm_distance;
}

/** 
 * Method is used to reset the lookup item values
 */
void LookupItem::init()
{
	mm_distance = 0;
}