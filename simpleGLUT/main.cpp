
//-----------------------------------------------------------------------------
// includes


#include <stdio.h>
#include <vector>
#include "gl/glut.h"
#include "utils.h"
#include "ply.h"
#ifdef WIN32
#include <windows.h>
#endif

// QL --------------------------------------------
// this is the function to find triangles from a list of points:
unsigned int *BuildTriangleIndexList (void *pointList, float factor, int numberOfInputPoints,
                              int numDimensions, int clockwise, int *numTriangleVertices);


JAMESDATA* j_data=NULL;
int j_data_len;
std::vector<JAMESDATA> drawData;
bool bShow = false;
unsigned short  nShow=0; //0 is original, 1 is z threshold, 2 is mesh;

#define X_RANGE 600
#define Z_RANGE 100
#define NUM_DIMENSIONS    2

#define DIST_THRESHOLD 800

 int numTriangleVertices;                                     // this does not need initialization
 float* testPointsXY ;   // to hold a set of random points

 unsigned int *triangleIndexList;   // this does not need initialization



//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// GLUT callbacks and functions

void initGlut(int argc, char **argv);
void displayFunc(void);
void idleFunc(void);
void reshapeFunc(int width, int height);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
//-----------------------------------------------------------------------------

// other [OpenGL] functions
void countFrames(void);
void renderBitmapString(float x, float y, float z, void *font, char *string);


//-----------------------------------------------------------------------------

bool bUsePredefinedCamera = true;

bool bFullsreen = false;
int nWindowID;

// camera attributes
float viewerPosition[3]		= { 0.0, 0.0, -500.0 };

//float viewerPosition[3]		= { 0.0, 0.0, -50.0 };

float viewerDirection[3]	= { 0.0, 0.0, 0.0 };
float viewerUp[3]			= { 0.0, 1.0, 0.0 };

// rotation values for the navigation
float navigationRotation[3]	= { 0.0, 0.0, 0.0 };

//-----------------------------------------------------------------------------

// parameters for the framecounter
char pixelstring[30];
int cframe = 0;
int time = 0;
int timebase = 0;

//-----------------------------------------------------------------------------

// parameters for the navigation

// position of the mouse when pressed
int mousePressedX = 0, mousePressedY = 0;
float lastXOffset = 0.0, lastYOffset = 0.0, lastZOffset = 0.0;
// mouse button states
int leftMouseButtonActive = 0, middleMouseButtonActive = 0, rightMouseButtonActive = 0;
// modifier state
int shiftActive = 0, altActive = 0, ctrlActive = 0;



//-----------------------------------------------------------------------------
bool init = false;
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

void displayFunc(void) {


	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, 1.33, 1.0, 100000.0);
	
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	//gluLookAt(	viewerPosition[0], viewerPosition[1], viewerPosition[2], 
	//			viewerDirection[0], viewerDirection[1], viewerDirection[2], 
	//			viewerUp[0], viewerUp[1], viewerUp[2]);

	// use lighting the color does not matter
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat	lightpos[4] = { 5.0, 15.0, 10.0, 1.0 }; 
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	
	// object translate so it is always negative 

	glTranslatef( viewerPosition[0], viewerPosition[1], viewerPosition[2] );
	// add navigation rotation

	glRotatef( navigationRotation[0], 1.0f, 0.0f, 0.0f );
	glRotatef( navigationRotation[1], 0.0f, 1.0f, 0.0f );

	//glutSolidTeapot(10.0);
	//j_data_len = 0;
	
	

	if(j_data_len > 0)
	{
  
    glPointSize(2.0f);//set point size to 10 pixels

	if(bShow)
	{

		int l,t;
		unsigned int lineIndex[6];




		for (l=0, t=0; t<numTriangleVertices; t += 3,l=0)  {
		
		
			
			// Each triangle has 3 lines, so D3D_PRIMITIVE_TOPOLOGY_LINELIST needs 6 vertices
		   // Each vertex has to be listed twice
		   lineIndex[l] = triangleIndexList[t];      l++;
		   lineIndex[l] = triangleIndexList[t+1];    l++;
		   lineIndex[l] = triangleIndexList[t+1];    l++;
		   lineIndex[l] = triangleIndexList[t+2];    l++;
		   lineIndex[l] = triangleIndexList[t+2];    l++;
		   lineIndex[l] = triangleIndexList[t];      l++;
		   


		   	glLineWidth(2.5); 
		
			glBegin(GL_LINES);
		   for(int k=0;k<6;k+=2)
		   {
			  
				if(lineIndex[k] >= drawData.size() || lineIndex[k+1] >= drawData.size())
					continue;

				float dist=0;
				
					dist += (drawData[lineIndex[k]].x-drawData[lineIndex[k+1]].x)*(drawData[lineIndex[k]].x-drawData[lineIndex[k+1]].x);

					dist += (drawData[lineIndex[k]].y-drawData[lineIndex[k+1]].y)*(drawData[lineIndex[k]].y-drawData[lineIndex[k+1]].y);

					dist += (drawData[lineIndex[k]].z-drawData[lineIndex[k+1]].z)*(drawData[lineIndex[k]].z-drawData[lineIndex[k+1]].z);

					if(dist > DIST_THRESHOLD)
						continue;
					
				glVertex3f(drawData[lineIndex[k]].x, drawData[lineIndex[k]].y, drawData[lineIndex[k]].z);
				glVertex3f(drawData[lineIndex[k+1]].x, drawData[lineIndex[k+1]].y, drawData[lineIndex[k+1]].z);


			
		   }
	glEnd();
		}

			/*for (auto it = std::begin(drawData); it!=std::end(drawData); ++it)
		{
			float p[3];
			 p[0] = it->x;
			 p[1] = it->y;
			 p[2] = it->z;
       	
			 
			 glBegin(GL_LINES);
			 glVertex3f(0.0, 0.0, 0.0);
			 glVertex3f(15, 0, 0);
			 glEnd();
			 
			 glBegin(GL_POINTS);
			 glPointSize(100);
			 glVertex3f(p[0],p[1],p[2]);
			 glEnd();
		}*/
	}
	else
	{
		for ( int i =0; i< j_data_len;i++)
		{
			 float p[3];
			 p[0] = j_data[i].x;
			 p[1] = j_data[i].y;
			 p[2] = j_data[i].z;
       	
		
			 glBegin(GL_POINTS);
			 glPointSize(100);
			 glVertex3f(p[0],p[1],p[2]);
			 glEnd();

		}

	}
    
	

	}
	else

		glutSolidTeapot(10.0);

	//countFrames();

	glutSwapBuffers();
}

//-----------------------------------------------------------------------------

void initGlut(int argc, char **argv) {

	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (640*2, 480*2);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	nWindowID = glutCreateWindow ("simpleGLUT");

	// Register callbacks:
	glutDisplayFunc		(displayFunc);
	glutReshapeFunc		(reshapeFunc);
	glutKeyboardFunc	(keyboardFunc);
	glutSpecialFunc		(specialFunc);
	glutMouseFunc		(mouseFunc);
	glutMotionFunc		(mouseMotionFunc);
	glutIdleFunc		(idleFunc);
}



//-----------------------------------------------------------------------------

void idleFunc(void) {
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------

void reshapeFunc(int width, int height) {

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0, 1.33, 1.0, 1000.0);

	glMatrixMode(GL_MODELVIEW);

}

//-----------------------------------------------------------------------------


// mouse callback
void mouseFunc(int button, int state, int x, int y) {
	
	// get the modifiers
	switch (glutGetModifiers()) {

		case GLUT_ACTIVE_SHIFT:
			shiftActive = 1;
			break;
		case GLUT_ACTIVE_ALT:
			altActive	= 1;
			break;
		case GLUT_ACTIVE_CTRL:
			ctrlActive	= 1;
			break;
		default:
			shiftActive = 0;
			altActive	= 0;
			ctrlActive	= 0;
			break;
	}

	// get the mouse buttons
	if (button == GLUT_LEFT_BUTTON)
		if (state == GLUT_DOWN) {
			leftMouseButtonActive += 1;
		} else
			leftMouseButtonActive -= 1;
	else if (button == GLUT_MIDDLE_BUTTON)
		if (state == GLUT_DOWN) {
			middleMouseButtonActive += 1;
			lastXOffset = 0.0;
			lastYOffset = 0.0;
		} else
			middleMouseButtonActive -= 1;
	else if (button == GLUT_RIGHT_BUTTON)
		if (state == GLUT_DOWN) {
			rightMouseButtonActive += 1;
			lastZOffset = 0.0;
		} else
			rightMouseButtonActive -= 1;

//	if (altActive) {
		mousePressedX = x;
		mousePressedY = y;
//	}
}

//-----------------------------------------------------------------------------

void mouseMotionFunc(int x, int y) {
	
	float xOffset = 0.0, yOffset = 0.0, zOffset = 0.0;

	// navigation
//	if (altActive) {
	
		// rotatation
		if (leftMouseButtonActive) {

			navigationRotation[0] += ((mousePressedY - y) * 180.0f) / 200.0f;
			navigationRotation[1] += ((mousePressedX - x) * 180.0f) / 200.0f;

			mousePressedY = y;
			mousePressedX = x;

		}
		// panning
		else if (middleMouseButtonActive) {

			xOffset = (mousePressedX + x);
			if (!lastXOffset == 0.0) {
				viewerPosition[0]	-= (xOffset - lastXOffset) / 8.0;
				viewerDirection[0]	-= (xOffset - lastXOffset) / 8.0;
			}
			lastXOffset = xOffset;

			yOffset = (mousePressedY + y);
			if (!lastYOffset == 0.0) {
				viewerPosition[1]	+= (yOffset - lastYOffset) / 8.0;
				viewerDirection[1]	+= (yOffset - lastYOffset) / 8.0;	
			}	
			lastYOffset = yOffset;

		}
		// depth movement
		else if (rightMouseButtonActive) {
			zOffset = (mousePressedX + x);
			if (!lastZOffset == 0.0) {
				viewerPosition[2] -= (zOffset - lastZOffset) / 5.0;
				viewerDirection[2] -= (zOffset - lastZOffset) / 5.0;
			}
			lastZOffset = zOffset;
		}
//	}

}

//-----------------------------------------------------------------------------

void keyboardFunc(unsigned char key, int x, int y) {
    
	switch(key) {
		
		// -----------------------------------------

#ifdef WIN32
		// exit on escape
		case '\033':
			exit(0);
			break;
#endif

		// -----------------------------------------
			
		// switch to fullscreen
		case 'f':

			bFullsreen = !bFullsreen;
			if(bFullsreen) 
				glutFullScreen();
			else {
				glutSetWindow(nWindowID);
				glutPositionWindow(100, 100);
				glutReshapeWindow(640*2, 480*2);
			}
			break;

		// -----------------------------------------
		case 's':
			bShow = !bShow;

			SavePLYfile("test",triangleIndexList, drawData, numTriangleVertices, DIST_THRESHOLD);
			break;
	}
}

//-----------------------------------------------------------------------------

void specialFunc(int key, int x, int y) {
	//printf("key pressed: %d\n", key);
}

//-----------------------------------------------------------------------------

void countFrames(void)  {

	time=glutGet(GLUT_ELAPSED_TIME);
	cframe++;
	if (time - timebase > 50) {
		sprintf(pixelstring, "fps: %4.2f", cframe*1000.0/(time-timebase));		
		timebase = time;
		cframe = 0;
	// Draw status text and uni-logo:
	} 
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	//glColor4f(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 200, 0, 200);
	glMatrixMode(GL_MODELVIEW);

	// render the string
	renderBitmapString(5,5,0.0,GLUT_BITMAP_HELVETICA_10,pixelstring);
		
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------

void renderBitmapString(float x, float y, float z, void *font, char *string) {
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

//-----------------------------------------------------------------------------

JAMESDATA* ReadJameData(char* filename, JAMESDATA* data)
{
	FILE* fp = fopen(filename,"r");
	int num = 0;
	fscanf(fp,"%d",&num);
	j_data_len = num;
	int i =0 ;

	data = new JAMESDATA [num];

	for(i=0;i<num;i++)
	{
		for(int j=0;j<4;j++)
		{
			char str[100];
			fscanf(fp,"%s",str);
			float ftemp = atof(str);
			switch(j)
			{
			case 0:
				data[i].x = ftemp;

				break;
			case 1:
				data[i].y = ftemp;
				break;
			case 2:
				data[i].z = ftemp;

				break;
			case 3:
				data[i].status = (int)ftemp;

				break;

			}
		}
	int b = 0;
	}


	

    fclose(fp);
	return data;
}

void ProcessJameData( JAMESDATA* data)
{
	if (data== NULL) return;

	float maxx = data[0].x;
	float maxy = data[0].y;
	float maxz = data[0].z;

	float minx = data[0].x;
	float miny = data[0].y;
	float minz = data[0].z;

	for(int i=0 ;i < j_data_len; i++)
	{
		if(minx > data[i].x) minx = data[i].x;
		if(miny > data[i].y) miny = data[i].y;
		if(minz > data[i].z) minz = data[i].z;

		if(maxx < data[i].x) maxx = data[i].x;
		if(maxy < data[i].y) maxy = data[i].y;
		if(maxz < data[i].z) maxz = data[i].z;
		
	}

	for(int i=0 ;i < j_data_len; i++)
	{
		float l = (maxx-minx);
		float r =  (maxy-miny)/l; // suppose x,y have the same mm/cycle

		data[i].x = ((data[i].x -minx)/ (maxx-minx)-0.5)*X_RANGE;
		data[i].y = ((data[i].y -miny)/ (maxy-miny)-0.5)*X_RANGE*r;
		//data[i].z = ((data[i].z -minz)/ (maxz-minz)-0.5)*Z_RANGE;
		//data[i].z -= maxz;
		//data[i].z = 100;
		// remiove some noise according to z;
		if(data[i].z > maxz*0.2 && data[i].z < maxz*0.8)
			drawData.push_back(data[i]);
	}

	// use some cleaned data

	testPointsXY = new float [drawData.size()*NUM_DIMENSIONS];

	for(int i=0 ;i < drawData.size();i++)
	{
		testPointsXY[i*NUM_DIMENSIONS] = drawData[i].x;
		testPointsXY[i*NUM_DIMENSIONS+1] = drawData[i].y;
		//testPointsXY[i*NUM_DIMENSIONS+2] = drawData[i].z;
	}


	triangleIndexList =  BuildTriangleIndexList(
          (void*)testPointsXY,          // An array of random integers between 0 and MAX_RAND
          (float)1.0,
		  drawData.size(),       // the number of random points
          NUM_DIMENSIONS,        // 2, because the list is X and Y points, no Z values
          1,
          &numTriangleVertices);

	int a = numTriangleVertices;

}



void main(int argc, char **argv) {
	
	j_data = ReadJameData("modbus-x-841-h-1000-January-23.txt",j_data);

	ProcessJameData( j_data);



	initGlut(argc, argv);

	glutMainLoop();
		
}



