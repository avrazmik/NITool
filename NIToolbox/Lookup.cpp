#include "stdafx.h"
#include "Lookup.h"

/** 
 * Default constructor 
 */
Lookup::Lookup(int width, int height, int depth)
{	
	this->width = width;
	this->height = height;
	this->depth = depth;

	map = new LookupItem**[this->width];
	for (int i = 0; i < this->width; ++i) 
	{
		map[i] = new LookupItem*[this->height];

		for (int j = 0; j < this->height; ++j)
			map[i][j] = new LookupItem[this->depth];
	}
}

/** 
 * Destructor
 */
Lookup::~Lookup(void)
{	
	for (int i = 0; i < this->width; ++i) {
		for (int j = 0; j < this->height; ++j)
		  delete [] map[i][j];

		delete [] map[i];
	}

	delete [] map;
}

/** 
 * Method is used to get the value cooresponding to the input parameters
 * @return LookupItem *	Pointer to the item
 */
LookupItem * Lookup::At(int x, int y, int d)
{
	return &map[x][y][d];
}

/** 
 * Method is used to set the width value
 * @param int	The value to set
 * @return void
 */
void Lookup::setWidth(int w)
{
	width = w;
}

/** 
 * Method is used to set the height value
 * @param int	The value to set
 * @return void
 */
void Lookup::setHeight(int h)
{
	height = h;
}

/** 
 * Method is used to set the depth value
 * @param int	The value to set
 * @return void
 */
void Lookup::setDepth(int d)
{
	depth = d;
}

/** 
 * Method is used to get the width value
 * @return int
 */
int Lookup::getWidth()
{
	return width;
}

/** 
 * Method is used to get the height value
 * @return int
 */
int Lookup::getHeight()
{
	return height;
}

/** 
 * Method is used to get the depth value
 * @return int
 */
int Lookup::getDepth()
{
	return depth;
}

/** 
 * Method is used to save the lookup table in a file system
 * @param char *	The full file name
 * @return void
 */
void Lookup::save(char *filename)
{
	ofstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "Notice: Cannot save, file is not opened" << endl;
		exit(1);
	}

	for(int z=0; z < this->getDepth(); z++)
	{
		for(int i=0; i < this->getHeight(); i++)
		{
			for(int j=0; j < this->getWidth(); j++)
			{
				if(this->At(j, i, z)->value() > 0)
					file<<j<<" "<<i<<" "<<z<<" "<<this->At(j, i, z)->value()<<endl;
			}
		}
	}

	file.close();

	cout<<"File closed successfully"<<endl;
}

/** 
 * Method is used to save the average errors in a file system
 * @param char *	The full file name
 * @return void
 */
void Lookup::saveErrors(char *filename)
{
	ofstream file;
	file.open(filename);
	if (!file.is_open()) {
		cerr << "Notice: Cannot save, file is not opened" << endl;
		exit(1);
	}

	float *errors = new float[this->getDepth()];
	for(int z=0; z < this->getDepth(); z++)
	{
		float avg_error = 0.0;
		int valid_count = 0;
		for(int i=0; i < this->getHeight(); i++)
		{
			for(int j=0; j < this->getWidth(); j++)
			{
				if(this->At(j, i, z)->value() > 0)
				{
					avg_error += abs(z - this->At(j, i, z)->value()); 
					valid_count++;
				}
			}
		}

		if(valid_count > 0)
			errors[z] = avg_error / valid_count;
	}
	/*
	int scale_mm = 100;
	int cur_index = 0;
	float average_error = 0.0;
	int error_count = 0;
	*/
	for(int i=0; i < depth; i++)
	{
		/*
		average_error += errors[i];
		error_count++;
		int ratio = i / scale_mm;
		if(cur_index != ratio)
		{
			cur_index = ratio;
			cout<<ratio * scale_mm<<" - "<<average_error / error_count<<endl;
			average_error = 0.0;
			error_count = 0;
		}	
		*/ 
		if(errors[i] <= 0) continue;
			file<<i<<" "<<errors[i]<<endl;
	}

	file.close();
}

/** 
 * Method is used to load the lookup table from the file
 * @param char *	The full file name
 * @return void
 */
void Lookup::load(char *filename)
{
	ifstream indata;
	indata.open(filename);

	if(!indata) {
      cerr << "Notice: file could not be opened" << endl;
    }

	string line;
	while (getline(indata, line))
	{
		int w,h,z, value;
		sscanf(line.c_str(),"%u %u %u %u", &w, &h, &z, &value);
		this->At(w, h, z)->value(value);
	}

	indata.close();
}