// Michael Chen
// Rope Simulation
// Programming Project # 3
// 4/11/18
 
#include <math.h>
#include <stdio.h>
#include <time.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif
 
 
 
 
#define BALL_POSITION_X 6
#define BALL_POSITION_Y -2
#define BALL_POSITION_Z 0
#define BALL_RADIUS 0.75
#define TRUE 1
#define FALSE 0

// Window sizes
#define WIDTH 1280
#define HEIGHT 720
 
// Define variables
#define NUM_OBJECTS 10
#define NUM_SPRINGS 9
#define SPRING_K    0.2 	// Spring constant
#define SPRING_D    0.05		// Damping constant
#define GRAVITY	    -0.098     // testing out values for gravity
#define PARTICLE_MASS  1

#define SPRING_MAX_LENGTH 2.0
#define SPRING_MIN_LENGTH 0.5

#define TIMESTEP 0.001
#define NUM_ITERATIONS 50
float oldtime,newtime;
 
// Define structure types that are used in Rope simulation
 
typedef struct vec3{
        float x;
        float y;
        float z;
     
} vec3;
 
typedef struct Particle{
    	vec3 position;	// Position
	vec3 velocity;	// velocity
	float Speed;	// Magnitude of Velocity
	vec3 forces;	// total force acting on the particle
	vec3 acceleration; // acceleration
	int fixed;	// fixed is a variable for the balls to not change position.
} Particle;
 
typedef struct Spring   {
    int End1; // First particle that spring is attached to
    int End2; // Second particle that spring is attached to
    float k;  // Spring constant
    float d;  // Damping constant
    float InitialLength;    // Unstretched length of spring
    float MinLength;	    // min length of spring = totally compressed
    float MaxLength;	    // max length of spring = totally stretched
} Spring;
 
Particle Objects[NUM_OBJECTS];
Particle OldObjects[NUM_OBJECTS];
Spring Springs[NUM_SPRINGS];
 
 
// *************************************************************************************
// * GLOBAL variables. Not ideal but necessary to get around limitatins of GLUT API... *
// *************************************************************************************
int pause = TRUE;
 
 
 
 
 
void init (void)
{
  glShadeModel (GL_SMOOTH);
  glClearColor (0.2f, 0.2f, 0.4f, 0.5f);                
  glClearDepth (1.0f);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glEnable (GL_COLOR_MATERIAL);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  GLfloat lightPos[4] = {-1.0, 1.0, 0.5, 0.0};
  glLightfv (GL_LIGHT0, GL_POSITION, (GLfloat *) &lightPos);
  glEnable (GL_LIGHT1);
  GLfloat lightAmbient1[4] = {0.0, 0.0,  0.0, 0.0};
  GLfloat lightPos1[4]     = {1.0, 0.0, -0.2, 0.0};
  GLfloat lightDiffuse1[4] = {0.5, 0.5,  0.3, 0.0};
  glLightfv (GL_LIGHT1,GL_POSITION, (GLfloat *) &lightPos1);
  glLightfv (GL_LIGHT1,GL_AMBIENT, (GLfloat *) &lightAmbient1);
  glLightfv (GL_LIGHT1,GL_DIFFUSE, (GLfloat *) &lightDiffuse1);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
 
    // Set particle locations from left to right
    int i; // iterator
    int xlength;    // length of spring along the x-axis
    int ylength;    // length of spring along the y-axis
     
    for (i = 0; i < NUM_OBJECTS; i++)    {
        Objects[i].position.x = 1.5*i;
        Objects[i].position.y = 0;
	Objects[i].position.z = 0;
	
	OldObjects[i].position.x = 1.5*i;
        OldObjects[i].position.y = 0;
	OldObjects[i].position.z = 0;

	Objects[i].acceleration.x = 0;
	Objects[i].acceleration.y = 0;
	Objects[i].acceleration.z = 0;

	
	// Set first and last ball as fixed
	if ( i == 0)
		Objects[i].fixed = 1;
	else if ( i == NUM_OBJECTS - 1)
		Objects[i].fixed = 1;
	else
		Objects[i].fixed = 0;
 
    }
    // Initialize springs connecting
    for (i = 0; i < NUM_SPRINGS; i++)    {
        Springs[i].End1 = i;
        Springs[i].End2 = i + 1;
         
        // Calculate the length of the spring
        xlength = Objects[i+1].position.x - Objects[i].position.x;
        ylength = Objects[i+1].position.y - Objects[i].position.y;
        Springs[i].InitialLength = (float)sqrt(xlength*xlength + ylength*ylength);
     	Springs[i].MinLength = Springs[i].InitialLength / 2;
	Springs[i].MaxLength = Springs[i].InitialLength * 1.5;
        Springs[i].k = SPRING_K;
        Springs[i].d = SPRING_D;
        
	
    }
 
 
}
 


// Update the rope simulation.
void update (void)
{
    	double dt = TIMESTEP; // set dt as timestep
	int i;
	double f, dl;
	vec3 pt1, pt2; 
	int j;	// counter
	vec3 r; // relative distance
	vec3 F;
	vec3 v1, v2, vr; // velocity
	
	// Initialize the spring forces and acceleration on each particle to zero
	for (i = 0; i < NUM_OBJECTS; i++)	{
		Objects[i].forces.x = 0;
		Objects[i].forces.y = 0;
		Objects[i].forces.z = 0;
		Objects[i].acceleration.x = 0;
		Objects[i].acceleration.y += GRAVITY;
		Objects[i].acceleration.z = 0;
	}

	// Calculate all spring forces based on position of connected objects
int a;
for (a = 0; a < 5; a++)	{
	for (i = 0; i < NUM_SPRINGS; i++)	{
		// Get the Values of the first end of the spring
		j = Springs[i].End1;
		pt1.x = Objects[j].position.x;
		pt1.y = Objects[j].position.y;
		pt1.z = Objects[j].position.z;
		
		// Get the values of the second end of the spring
		j = Springs[i].End2;
		pt2.x = Objects[j].position.x;
		pt2.y = Objects[j].position.y;
		pt2.z = Objects[j].position.z;
		
		
		// calculate relative distance between 2 ends of spring
		r.x = pt2.x - pt1.x;
		r.y = pt2.y - pt1.y;
		r.z = pt2.z - pt2.z;
		
/*
		float current_distance = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);

		vec3 correction_vector;
		correction_vector.x = r.x * (1.0f - Springs[i].InitialLength/current_distance);
		correction_vector.y = r.y * (1.0f - Springs[i].InitialLength/current_distance);
		correction_vector.z = r.z * (1.0f - Springs[i].InitialLength/current_distance);

		if ( Objects[i].fixed == 0 )	{
			Objects[i].position.x = pt1.x + 0.5 * correction_vector.x;
			Objects[i].position.y = pt1.y + 0.5 * correction_vector.y;
			Objects[i].position.z = pt1.z + 0.5 * correction_vector.z;
		}

		if ( Objects[j].fixed == 0 )	{
			Objects[j].position.x = pt1.x - 0.5 * correction_vector.x;
			Objects[j].position.y = pt1.y - 0.5 * correction_vector.y;
			Objects[j].position.z = pt1.z - 0.5 * correction_vector.z;
		}
*/
		// calculate change in spring length
		float length = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
		if (length >= SPRING_MAX_LENGTH)	{
			length = SPRING_MAX_LENGTH;
		}
		if ( length <= SPRING_MIN_LENGTH)	{
			length = SPRING_MIN_LENGTH;
		}
		dl = length - Springs[i].InitialLength;
		f = Springs[i].k * dl; // - means compression, + means tension
		
		// Unit Vector
		// Turn r into unit vector
		float unit = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
		r.x = r.x / unit;
		r.y = r.y / unit;
		r.z = r.z / unit;
		

		// calculate force on particle.
		// find out if under compression or tension
		//dl = length - Springs[i].InitialLength greaater or less
		if ( dl < 0 )	{	// Compression
			F.x = 1*(r.x*f) ;
			F.y = 1*(r.y*f) ;
			F.z = 1*(r.z*f) ;
		}
		if (dl > 0 )	{	// tension 
			F.x = -1*(r.x*f) ;
			F.y = -1*(r.y*f) ;
			F.z = -1*(r.z*f) ;
		}
		// Update Spring Forces
		j = Springs[i].End1;
		if (j == 0) 	{
			// Don't give spring forces if it is first particle.
		}
		else	{
			Objects[j].forces.x -= F.x /2;
			Objects[j].forces.y -= F.y /2;
			Objects[j].forces.z -= F.z /2;
		}
		
		j = Springs[i].End2;
		if ( j == NUM_OBJECTS - 1)	{
			// Don't give spring forces if it is last particle.
		}
		else	{
			Objects[j].forces.x += F.x /2;
			Objects[j].forces.y += F.y /2;
			Objects[j].forces.z += F.z /2;
		}
	}
}
	
	vec3 new_position;
	vec3 old_position;
	vec3 temp;
	// Integrate equations of motion (Verlet?)
	// Also check for collision with the ball.
	for ( i = 0; i < NUM_OBJECTS; i++)	{
		if ( Objects[i].fixed == 0 )	{
			// Update position 
			// x_new = x_current + v_current * time
			Objects[i].acceleration.x = Objects[i].acceleration.x + PARTICLE_MASS * Objects[i].forces.x;
			Objects[i].acceleration.y = Objects[i].acceleration.y + PARTICLE_MASS * Objects[i].forces.y;
			Objects[i].acceleration.z = Objects[i].acceleration.z + PARTICLE_MASS * Objects[i].forces.z;

			// gravity doesn't affect x axis
			Objects[i].velocity.x = Objects[i].velocity.x + Objects[i].acceleration.x * TIMESTEP; 
			new_position.x = Objects[i].position.x + Objects[i].velocity.x * TIMESTEP;
			
			Objects[i].velocity.y = Objects[i].velocity.y + Objects[i].acceleration.y * TIMESTEP;
			new_position.y = Objects[i].position.y + Objects[i].velocity.y * TIMESTEP;
			
			// gravity doesn't affect z axis
			Objects[i].velocity.z = Objects[i].velocity.z + Objects[i].acceleration.z * TIMESTEP; 
			new_position.z = Objects[i].position.z + Objects[i].velocity.z * TIMESTEP;
			


			// reset acceleration
			Objects[i].acceleration.x = 0;
			Objects[i].acceleration.y = 0;
			Objects[i].acceleration.z = 0;

			// check for collision.

			if( new_position.x >= BALL_POSITION_X - BALL_RADIUS && new_position.x <= BALL_POSITION_X + BALL_RADIUS && new_position.y >= BALL_POSITION_Y - BALL_RADIUS && new_position.y <= BALL_POSITION_Y + BALL_RADIUS )	{
				Objects[i].position.x = Objects[i].position.x;
				Objects[i].position.y = BALL_POSITION_Y + BALL_RADIUS;
			}
			else {
				Objects[i].position.x = new_position.x;
				Objects[i].position.y = new_position.y;
			}
		}
	
	}
	
	
	// Render the scene.	
    	glutPostRedisplay ();
}
 
 
 
void display (void)
{
  int x;
 
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
  glDisable (GL_LIGHTING);
  glBegin (GL_POLYGON);
  glColor3f (0.8f, 0.8f, 1.0f);
  glVertex3f (-200.0f, -100.0f, -100.0f);
  glVertex3f (200.0f, -100.0f, -100.0f);
  glColor3f (0.4f, 0.4f, 0.8f); 
  glVertex3f (200.0f, 100.0f, -100.0f);
  glVertex3f (-200.0f, 100.0f, -100.0f);
  glEnd ();
  glEnable (GL_LIGHTING);
  glTranslatef (-6.5, 6, -9.0f); // move camera out and center on the rope
  glPushMatrix ();
  glTranslatef (BALL_POSITION_X, BALL_POSITION_Y, BALL_POSITION_Z);
  glColor3f (1.0f, 1.0f, 0.0f);
  glutSolidSphere (BALL_RADIUS - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of rope penetrating the ball slightly
  glPopMatrix ();
     
    	// Draw the rope int its initial state
    	// Draw balls of the rope
    	int i;
	glEnable(GL_COLOR_MATERIAL);
   	glColor3f(1.0f, 1.0f, 1.0f);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
	glPointSize(10.0);
    	glBegin(GL_POINTS);
   
    	for (i = 0; i < NUM_OBJECTS; i++)    {   
        	glVertex3f(Objects[i].position.x, Objects[i].position.y, Objects[i].position.z);
         
    	}
    	glEnd();
         
    	// Draw springs between the balls
    	int j;
	glEnable(GL_COLOR_MATERIAL);
   	glColor3f(1.0f, 1.0f, 1.0f);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(5.0);
    	glBegin(GL_LINES);
    	for (j = 0; j < NUM_SPRINGS; j++)    {
			
        	glVertex3f(Objects[j].position.x, Objects[j].position.y, Objects[j].position.z);
		glVertex3f(Objects[j+1].position.x, Objects[j+1].position.y, Objects[j+1].position.z);
    	}
	glEnd();
     	glFlush();
  glutSwapBuffers();
  glutPostRedisplay();
	if(!pause)	{
		update();
		oldtime = newtime;
	}
	newtime = glutGet(GLUT_ELAPSED_TIME);
}
 

 
 
void reshape (int w, int h)  
{
  glViewport (0, 0, w, h);
  glMatrixMode (GL_PROJECTION); 
  glLoadIdentity ();  
  if (h == 0)  
  { 
    gluPerspective (80, (float) w, 1.0, 5000.0);
  }
  else
  {
    gluPerspective (80, (float) w / (float) h, 1.0, 5000.0);
  }
  glMatrixMode (GL_MODELVIEW);  
  glLoadIdentity (); 
}
 
 
 
 
 
void keyboard (unsigned char key, int x, int y) 
{
  switch (key) 
  {
    case 27:    // esc 
      exit (0);
    break;  
    case 32:    // space
      pause = 1 - pause;
      break;
    default: 
    break;
  }
}
 
 
 
 
 
void arrow_keys (int a_keys, int x, int y) 
{
  switch(a_keys) 
  {
    case GLUT_KEY_UP:
      glutFullScreen();
    break;
    case GLUT_KEY_DOWN: 
      glutReshapeWindow (WIDTH, HEIGHT );
    break;
    default:
    break;
  }
}
 
 
 
 
 
int main (int argc, char *argv[]) 
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
  glutInitWindowSize (WIDTH, HEIGHT ); 
  glutCreateWindow ("Rope simulator");
  init ();
  // glutIdleFunc (idle);
  glutDisplayFunc (display);  
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (arrow_keys);
  glutMainLoop ();
}
