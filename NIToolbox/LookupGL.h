#pragma once
#include <GL/glut.h>
#include <string>
#include "Lookup.h"

class LookupGL
{
public:
	LookupGL();
	LookupGL(Lookup *);
	~LookupGL(void);
	void visualize();
	static void displayLoop();
	static void reshape(int w, int h);
	static void specialKeys(int key, int x, int y);
	static void keyboard(unsigned char key, int x, int y);
	static void useMenu();
	static void handleMenuEvents(int);
	static double rotate_x;
	static double rotate_y; 
	static double rotate_z;
	static bool b_toggle_set;

	static void minify(Lookup *);
	static void setLookup(Lookup *);
	static void setLookupMinified(Lookup *);
	static Lookup * getLookup();
	static Lookup * getLookupMinified();
	static Lookup *pLookup;
	static Lookup *pMinified;	
	static char *lookupPath;
};

