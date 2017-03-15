/* 
 * Demo 1
 * Asteroids
 * Student
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "shared/gltools.h"	// OpenGL toolkit
using namespace std;

//set up constants
const int numOfStars = 100;		//sets number of stars
const int numOfAsteroids = 6;	//sets number of asteroids
float xmin = -50.0, xmax = 50.0, ymin = -50.0, ymax = 50.0;		//for coordinate limits

//set up variables
float shipAngle = 0;
float bulletAngle = 0;

float frameStart = 0;	//set up time variables
float frameEnd = 0;

float bulletX = 0.0;		//bullet coord init
float bulletY = 0.0;
float starX[numOfStars];	//arrays for to store star coords
float starY[numOfStars];
float asteroidX[numOfAsteroids];	//arrays to store asteroid coords
float asteroidY[numOfAsteroids];

bool bulletIsAlive = false;			//to detect if bullet exists
bool astIsAlive[numOfAsteroids];	//to detect if asteroids exist

char scoreStr[5];	//char array to store score variable for conversion to string
int score = 0;		//score init

int numAstDead = 0;	//to detect how many asteroids have been destroyed

//Declare functions

float random(int range) //create a random number in a specified range
{	
	float randNum;
	randNum = (rand() % range);
	return randNum;
}

bool hasCollided(float asteroidX, float asteroidY)	//collision detection for ship and asteroids when asteroid coords initialise
{
	int asteroidWidth = 11;		//bounds for asteroid and ship bounding boxes
	int asteroidHeight = 10;
	int shipWidth = 6;
	int shipHeight = 6;

	//Takes an absolute value of x and y difference between ship and asteroid and compares with bounding boxes of both
	if ((abs(0.0 - asteroidX) < ( shipWidth + asteroidWidth) / 2) && (abs(0.0 - asteroidY) < (shipHeight + asteroidHeight) / 2)) 
	{
		cout << "Collision" << endl;		//cout statements to ensure operation of ship-asteroid collision
		return true;
	}
	else
	{
		cout << "No collision" << endl;
		return false;
	}
}

void setUpStarCoords()	//to initialise array of random coords for star within coordinate space
{
	for ( int i=0; i < numOfStars; i++)
	{
		starX[i] = (random(100) - xmax);
		starY[i] = (random(100) - ymax);
	}
}

void setUpAsteroidCoords() //to initialise array of random coords for asteroids within coordinate space
{
	float x, y;

	for (int i = 0; i < numOfAsteroids; i++)
	{
		x = (random(100) - xmax);
		y = (random(100) - ymax);
		
		if (hasCollided(x, y))	//if collision with ship exists, coords are initialised again
		{
			i--;
		}
		else
		{
			asteroidX[i] = x;
			asteroidY[i] = y;
		}
	}
}

void renderString(float x, float y, float z, void *font, char *string) //draws a string to the screen. Used for score
{  
	glColor3f(1.0,0.0,0.0);
    char *c;
    glRasterPos3f(x,y,z);
    for (c=string; *c != '\0'; c++) 
	{
        glutBitmapCharacter(font, *c);
    }
}

bool isInBounds(float x, float y) //checks if given coords are contained within the coordinate system. 
{		
	if ((x > xmin) && (x < xmax) && (y > ymin) && (y < ymax))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

void astHitCheck () //checks whether bullet collides with asteroid
{		
	if (bulletIsAlive)
	{
	for (int i=0; i < numOfAsteroids; i++)	//checks if bullet collides within asteroid bounding boxes
		{
		if((astIsAlive[i] == true) && (bulletX > (asteroidX[i] - 6.0)) && (bulletX < (asteroidX[i] + 5.0)) && (bulletY > (asteroidY[i] - 5.0)) && (bulletY < (asteroidY[i] + 5.0))) 
			{
			astIsAlive[i] = false;	//records asteroid as destroyed
			bulletIsAlive = false;	//records bullet as destroyed
			score = score + 10;		//increases score
			numAstDead++;			//records number of asteroids destroyed
			}
		}
	}	
}

void setAstAlive()		//initialises all asteroids as alive
{
	for ( int i=0; i<numOfAsteroids; i++)
	{
		astIsAlive[i] = true;
	}
}

// reshape callback function for whenever screen is moved or resized
void reshape(GLint width, GLint height)
{
   glViewport(0, 0, width, height);
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
  
   glOrtho(xmin, xmax, ymin, ymax, -1.0, 1.0);   //sets coordinate space
   glMatrixMode( GL_MODELVIEW );
}

void drawStar()		//draws stars in coordinate space using coords from arrays
{
    glPointSize(2.0);	//sets size of stars
	glLoadIdentity();

	for (int i=0; i<numOfStars; i++)
	{
		glBegin(GL_POINTS);
		glVertex2f(starX[i], starY[i]);
		glEnd();
	}
}

void moveStar(float starSpeed, float time)	//for moving star field
{
	for (int i=0; i<numOfStars; i++)
	{
		if (starY[i] > ymin)			//tests if stars are still in coordinate space and moves them 
		{
			starY[i] = (starSpeed*time) + starY[i];
		} 
		else if (starY[i] < ymin)		//when stars leave bottom of the screen, redraws them at the top of the screen with a random x coordinate
		{
			starX[i] = random(100) - xmax;
			starY[i] = ymax;
		}			
	}
}

void drawShip(GLfloat shipX, GLfloat shipY) 
{
   glLoadIdentity();
   glRotatef( shipAngle, 0.0, 0.0, 1.0 );   //rotates the ship dependant on angle set by keyboard functions 

   glBegin(GL_POLYGON);						//draws spaceship at the origin of world coords
      glVertex2f( shipX, shipY + 3.0 );
      glVertex2f( shipX + 2.0, shipY - 3.0 );
	glColor3f(0.2,0.2,0.);					//sets colour blend
      glVertex2f( shipX, shipY - 2.0);
      glVertex2f( shipX - 2.0, shipY - 3.0);
   glEnd();
}

void drawBullet()		//draws bullet on screen
{
	glPointSize(4.0);

	glBegin(GL_POINTS);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(bulletX,bulletY);

	glEnd();
}

void moveBullet(float bulletSpeed, float time)		//moves bullet using given velocity and time variables
{
	float angleRad = 0;
	float pi = 3.14159;

	if (isInBounds(bulletX, bulletY))				//to check bullet is within coord space
	{
		angleRad = ((bulletAngle+90)/180)*pi;		//converts angle of bullet from degrees to radians
		bulletX = ((bulletSpeed * time) * cos(angleRad)) + bulletX;		//trigonometry calculations to set direction angle relative to angle of ship
		bulletY = ((bulletSpeed * time) * sin(angleRad)) + bulletY;		
	}
	else
	{
		bulletIsAlive = false;
	}
}

void drawAsteroid()			//draws asteroids to screen
{
	glLoadIdentity();
	for (int i=0; i<numOfAsteroids; i++) 
	{
		if (astIsAlive[i] == true)
		{
		glBegin(GL_POLYGON);		//Specifies colour and vertices for asteroids
		glColor3f(1.0,0.0,1.0);
			glVertex2f( asteroidX[i], asteroidY[i] + 5.0 );
			glVertex2f( asteroidX[i] + 5.0, asteroidY[i] + 2.0 );
		glColor3f(0.0,0.0,0.5);		//for colour blend
			glVertex2f( asteroidX[i] + 4.0, asteroidY[i] - 3.0 );
			glVertex2f( asteroidX[i] - 3.0, asteroidY[i] - 5.0 );
			glVertex2f( asteroidX[i] - 4.0, asteroidY[i] - 1.0 );
			glVertex2f( asteroidX[i] - 6.0, asteroidY[i] );
		glEnd();
		}
	}	
}

void createBullet()		//initialises each new bullet
{
	bulletAngle = shipAngle;		//sets angle of bullet when fired
	bulletX = 0.0;					//sets bullet back to origin
	bulletY = 0.0;
	bulletIsAlive = true;			//bullet exists
}

// display callback function - everything is drawn to screen here
  
void display(void)
{
	frameStart = glutGet(GLUT_ELAPSED_TIME);			//gets time from last time function was called records it as time frame began
	float frameTime = (frameStart - frameEnd) / 1000;	//calculates time difference for each frame in milliseconds

	glClear (GL_COLOR_BUFFER_BIT);   // clear window 
	glColor3f(1.0, 1.0, 1.0);        // white drawing objects 

	drawStar(); 
	moveStar(-3.0,frameTime);
	
	if (bulletIsAlive)				//only draws and moves bullet if bullet exists
		{
		moveBullet(80.0,frameTime);
		drawBullet();
		}

	glColor3f(1.0, 1.0, 1.0); 
	drawShip(0.0,0.0);

	drawAsteroid();
	astHitCheck();

	renderString(-45,45, 0.0, GLUT_BITMAP_HELVETICA_12, "Score: " );
	itoa(score, scoreStr, 10);			//converts score integer to string and stores each character in char array
	renderString(-38,45, 0.0, GLUT_BITMAP_HELVETICA_12, scoreStr );

	glFlush();	// execute drawing commands in buffer

	if (numAstDead == numOfAsteroids) //tests if all asteroids are destroyed and reinitialises if so
	{
	   setUpAsteroidCoords();
	   setAstAlive();
	   numAstDead = 0;
	}

	frameEnd = frameStart;			//gets time from last time function was called records it as time frame ends
}

// initialisation function at program start
void init(void)
{
   srand (time(NULL));		// sets seed for random number use
   setUpStarCoords();		
   setUpAsteroidCoords();		
   setAstAlive();
   glClearColor (0.0, 0.0, 0.0, 0.0);   //window background set to black
}

void keyboard(unsigned char key, int x, int y)	//normal keyboard callback function
{
	switch (key)
	{
	case 32:	if (!bulletIsAlive){		//creates a bullet when spacebar is pressed if one does not already exist
				createBullet();
				}
				break;

	default:	break;
	}
}

void spKeyboard(int key, int x, int y)	//special keyboard callback function
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:	shipAngle = shipAngle - 5.0;		//to change rotation of ship 
							break;
	case GLUT_KEY_LEFT:		shipAngle = shipAngle + 5.0;
							break;
	default:				break;
	}
}


int main(int argc, char** argv)
{	
/* window management code ... */
   /* initialises GLUT and processes any command line arguments */  
   glutInit(&argc, argv);
   /* use single-buffered window and RGBA colour model */
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   /* window width = 600 pixels, height = 600 pixels */
   glutInitWindowSize (600, 600);
   /* window upper left corner at (100, 100) */
   glutInitWindowPosition (100, 100);
   /* creates an OpenGL window with command argument in its title bar */
   glutCreateWindow ("W12939549 CW1 Asteroids");
   
   init();
   
   //registers callback functions	
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(spKeyboard);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(display);			//allows for timed animations 
   glutMainLoop();

   return 0;
}

