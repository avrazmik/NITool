#include "stdafx.h"
#include "LookupGL.h"

Lookup * LookupGL::pLookup  = NULL;
Lookup * LookupGL::pMinified  = NULL;

double LookupGL::rotate_x = -10;
double LookupGL::rotate_y = 20;
double LookupGL::rotate_z = 0;

char * LookupGL::lookupPath = NULL;
bool LookupGL::b_toggle_set = false;

LookupGL::LookupGL()
{
	
}

/** 
 * Default constructor
 * @param Lookup *	Pointer to the lookup table
 */
LookupGL::LookupGL(Lookup * p)
{
	this->setLookup(p);
}

/** 
 * Destructor
 */
LookupGL::~LookupGL(void)
{
}

void LookupGL::handleMenuEvents(int arg)
{
	if(arg == 1)
	{
		printf("Saving the lookup table...\n");
		pLookup->save(strcat(LookupGL::lookupPath, "depth.lookup"));
		printf("Done!\n");

		printf("Saving the errors...\n");
		pLookup->saveErrors(strcat(LookupGL::lookupPath, "errors.txt"));
		printf("Done!\n");

		exit(0);
	}

	else

	if(arg == 2)
	{
		exit(0);
	}
}

void LookupGL::useMenu()
{
	glutCreateMenu(LookupGL::handleMenuEvents);
	if(LookupGL::lookupPath != NULL)
		glutAddMenuEntry("Save the lookup", 1);

	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


/** 
 * Method is used to visualize the lookup table
 * @return void
 */
void LookupGL::visualize()
{
	int argc = 1;
	char* argv[1] = {""};
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow("Depth Lookup Visualization");
	//glEnable(GL_DEPTH_TEST);
	glutDisplayFunc(this->displayLoop);
	glutIdleFunc(this->displayLoop);
	glutReshapeFunc(this->reshape);
	glutSpecialFunc(this->specialKeys);
	glutKeyboardFunc(this->keyboard);
	useMenu();
	glutMainLoop(); 
}

/** 
 * Glut display function
 * @return void
 */
void LookupGL::displayLoop()
{
	Lookup *curr = LookupGL::pMinified == NULL ? LookupGL::pLookup : LookupGL::pMinified;
	
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix(); 
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glDisable( GL_DEPTH_TEST ) ;
	glColor3f(1, 1, 1);
	glRasterPos2f( -0.95, -0.75);

	char buf[20];
	sprintf_s( buf, "ResolutionX : %d", pLookup->getWidth()) ;
	const char * p = buf ;
	do glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *p ); while( *(++p) ) ;

	glRasterPos2f( -0.95, -0.85);
	sprintf_s( buf, "ResolutionY : %d", pLookup->getHeight());
	p = buf ;
	do glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *p ); while( *(++p) ) ;

	glRasterPos2f( -0.95, -0.95);
	sprintf_s( buf, "Depth : %d",  pLookup->getDepth()) ;
	p = buf ;
	do glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, *p ); while( *(++p) ) ;

	glMatrixMode(GL_PROJECTION) ;
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW) ;
	glPopMatrix();
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, -2.5f, 0.0f, 0.0f,  0.0f, 0.0f, 2.0f,  0.0f);

	int scale_factor = 100;
	
	GLdouble scaled_width = ((GLdouble) curr->getWidth())/scale_factor;
	GLdouble scaled_height = ((GLdouble) curr->getHeight())/scale_factor;
	GLdouble scaled_depth = ((GLdouble) curr->getDepth())/scale_factor;

	glPushMatrix();
	
		glRotatef(rotate_x, 1.0, 0.0, 0.0 );
		glRotatef(rotate_y, 0.0, 1.0, 0.0 );
		glRotatef(rotate_z, 0.0, 1.0, 0.0 );
	
		glColor3f(1, 0, 0);
		glPushMatrix();
			glScalef(scaled_width, scaled_height, scaled_depth);
			glutWireCube(1);
		glPopMatrix();
	
		glPushMatrix();
			glTranslatef(-(scaled_width/2), -(scaled_height/2), -(scaled_depth/2));	
			glColor3f(1, 1, 1);	

			glBegin(GL_LINES);
				//draw x with arrows
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(1.0, 0.0, 0.0);

				glVertex3f(1.0, 0.0, 0.0);
				glVertex3f(0.9, 0.0, 0.05);

				glVertex3f(1.0, 0.0, 0.0);
				glVertex3f(0.9, 0.0, -0.05);
			
				//draw y with arrows
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(0.0, 1.0, 0.0);
			
				glVertex3f(0.0, 1.0, 0.0);
				glVertex3f(0.05, 0.9, 0.0);

				glVertex3f(0.0, 1.0, 0.0);
				glVertex3f(-0.05, 0.9, 0.0);
			
				//draw z with arrows
				glVertex3f(0.0, 0.0, 0.0);
				glVertex3f(0.0, 0.0, 3.0);

				glVertex3f(0.0, 0.0, 3.0);
				glVertex3f(0.05, 0.0, 2.9);

				glVertex3f(0.0, 0.0, 3.0);
				glVertex3f(-0.05, 0.0, 2.9);
			
			glEnd();

			std::string depth = "(0.0.0)";
		
			glRasterPos3f(-0.09, -0.09, -0.09);
			for (int i = 0; i < depth.length(); i++)
			{
				glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, depth[i]);
			}

			glBegin(GL_POINTS);
		
			for(int z=0; z < curr->getDepth(); z++)
			{
				for(int i=0; i < curr->getHeight(); i++)
				{
					for(int j=0; j < curr->getWidth(); j++)
					{
						if(curr->At(j, i, z)->value() > 0)
						{
							if(!b_toggle_set)
							{
								glColor3f(0, 1, 0);
								glVertex3d((GLdouble) (j) / scale_factor, (GLdouble) (curr->getHeight() - i) / scale_factor, (GLdouble) z / scale_factor);						
							}
						
							else

							{
								glColor3f(1, 1, 0);
								glVertex3d((GLdouble)  (j) / scale_factor, (GLdouble) (curr->getHeight() - i) / scale_factor, (GLdouble) (curr->At(j, i, z)->value()) / scale_factor);
							}
						}
					}
				}
			}

			glEnd();
		glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
}

/** 
 * Glut special keys function
 * @param int
 * @param int
 * @param int
 * @return void
 */
void LookupGL::specialKeys(int key, int x, int y) 
{ 
	switch(key)
	{
		case GLUT_KEY_UP:
			LookupGL::rotate_x += 5;
			break;
		case GLUT_KEY_RIGHT:
			LookupGL::rotate_y += 5;
			break;
		case GLUT_KEY_DOWN:
			LookupGL::rotate_x -= 5;
			break;
		case GLUT_KEY_LEFT:
			LookupGL::rotate_y -= 5;
			break;
	}
 
	glutPostRedisplay();
}


/** 
 * Glut keyboard function
 * @param int
 * @param int
 * @param int
 * @return void
 */
void LookupGL::keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 's':
			LookupGL::handleMenuEvents(1);
			break;
		case 't':
			LookupGL::b_toggle_set = !LookupGL::b_toggle_set;
	}

	glutPostRedisplay();
}

/** 
 * Glut reshape function
 * @param int
 * @param int
 * @return void
 */
void LookupGL::reshape(int w, int h)
{
	if (h == 0) h = 1;
	float ratio =  w * 1.0 / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

/** 
 * Method is used to minify the lookup table
 * @param Lookup *	The pointer of the lookup table that has to be visualized
 * @return void
 */
void LookupGL::minify(Lookup *tbl)
{
	LookupGL::setLookup(tbl);

	int marching_cube_size = 10;
	Lookup *min = new Lookup(tbl->getWidth()/marching_cube_size, tbl->getHeight()/marching_cube_size, tbl->getDepth()/marching_cube_size);
	
	printf("Visualizing...\n");
	for(int i=0; i < min->getWidth(); i++)
	{
		for(int j=0; j < min->getHeight(); j++)
		{
			for(int d=0; d < min->getDepth(); d++)
			{
				int average_value = 0;
				int total_num = 0;
				for(int ii=i*marching_cube_size; ii < i*marching_cube_size + marching_cube_size; ii++)
				{
					for(int jj=j*marching_cube_size; jj < j*marching_cube_size + marching_cube_size; jj++)
					{
						for(int dd=d*marching_cube_size; dd < d*marching_cube_size + marching_cube_size; dd++)
						{
							if(tbl->At(ii, jj, dd)->value() > 0)
							{
								average_value += tbl->At(ii, jj, dd)->value();
								total_num++;
							}							
						}
					}
				}
				
				if(total_num > 0)
				{
					min->At(i, j, d)->value((average_value / total_num) / marching_cube_size);					
				}

				else

				{
					min->At(i, j, d)->value(0);
				}
			}
		}
	}

	LookupGL::setLookupMinified(min);
}

/** 
 * Method is used to set the lookup table
 * @param Lookup *	The pointer of the lookup table that has to be visualized
 * @return void
 */
void LookupGL::setLookup(Lookup *tbl)
{
	pLookup = tbl;
}

/** 
 * Method is used to get the lookup table
 * @return Lookup * The pointer of the lookup table
 */
Lookup * LookupGL::getLookup()
{
	return pLookup;
}

/** 
 * Method is used to set the minfied lookup table
 * @param Lookup *	The pointer of the minfied lookup table that has to be visualized
 * @return void
 */
void LookupGL::setLookupMinified(Lookup *tbl)
{
	pMinified = tbl;
}

/** 
 * Method is used to get the minfied lookup table
 * @return Lookup * The pointer of the minfied lookup table
 */
Lookup * LookupGL::getLookupMinified()
{
	return pMinified;
}
