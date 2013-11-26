#pragma once
class LookupItem
{
public:
	LookupItem(void);
	~LookupItem(void);
	void init();
	void value(unsigned short int);
	unsigned short int value();
private:
	unsigned short int mm_distance;
};

