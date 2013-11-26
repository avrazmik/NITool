#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include "LookupItem.h"

using namespace std;

class Lookup
{
public:
	Lookup(int, int, int);
	~Lookup(void);
	LookupItem * At(int, int, int);

	void setWidth(int);
	void setHeight(int);
	void setDepth(int);

	int getWidth();
	int getHeight();
	int getDepth();

	void save(char *);
	void saveErrors(char *);
	void load(char *);	
private:
	LookupItem ***map;
	int width;
	int height;
	int depth;	
};