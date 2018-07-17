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
#define BALL_POSITION_Y -70
#define BALL_POSITION_Z 0
#define BALL_RADIUS 0.75
#define TRUE 1
#define FALSE 0

#define GRAVITY -2.5
#define DAMPING 0.05
#define K_CONSTANT 20

#define PARTICLE_WIDTH 50
#define PARTICLE_HEIGHT 50
#define TOTAL_PARTICLES PARTICLE_WIDTH*PARTICLE_HEIGHT

#define SPRING_COUNT (PARTICLE_WIDTH-1)*PARTICLE_HEIGHT
#define SPRING_COUNT_DIAG (PARTICLE_WIDTH-1)*(PARTICLE_HEIGHT-1)
#define MASS 1
#define TIMESTEP 0.05
#define CONSTRAINT_ITERATIONS 10

// *************************************************************************************
// * GLOBAL variables. Not ideal but necessary to get around limitatins of GLUT API... *
// *************************************************************************************
int pause = TRUE;

// *******************
// * DATA STRUCTURES *
// *******************

typedef struct Vec3 {
	float x;
	float y;
	float z;
} Vec3;




typedef struct Particle	{
	int moveable; // 1 is movable, 0 is stationary.
	float mass;
	Vec3 position;
	Vec3 old_position;
	Vec3 acceleration;
	Vec3 normal;
} Particle;

typedef struct Spring	{
	int point1; // index of particle 1
	int point2; // index of particle 2
	float restDistance;
} Spring;

//*******************
//* VECTOR3 METHODS *
//*******************

// 
float magnitude ( Vec3 v )	{

	return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

Vec3 normalized( Vec3 v )	{
	float m = magnitude(v);
	Vec3 normalized_vec = { v.x/m, v.y/m, v.z/m };
	return normalized_vec;
}

Vec3 Cross( Vec3 v1, Vec3 v2 ) 	{
	Vec3 cross_vec = { (v1.y * v2.z) - (v1.z * v2.y),
		           (v1.z * v2.x) - (v1.x * v2.z),
		           (v1.x * v2.y) - (v1.y * v2.x) };
	return cross_vec;
}

float Dot( Vec3 v1, Vec3 v2 ) 	{
	float d = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return d;
}

//********************
//* PARTICLE METHODS *
//********************

Particle particles[TOTAL_PARTICLES];

Vec3 calculate_Normals(int index1, int index2, int index3)	{

	Vec3 v1 = { particles[index2].position.x - particles[index1].position.x,
		    particles[index2].position.y - particles[index1].position.y,
		    particles[index2].position.z - particles[index1].position.z };
	Vec3 v2 = { particles[index3].position.x - particles[index1].position.x,
		    particles[index3].position.y - particles[index1].position.y,
		    particles[index3].position.z - particles[index1].position.z };
		
	Vec3 normal = Cross(v1,v2);
	return normal;
}

void initialize_Normals()	{
	
	int x;
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		
		int right_check = x/PARTICLE_WIDTH + 1;
		
		// indexes for triangles to calc normals
		int right = x + 1;
		int down = x + PARTICLE_WIDTH;
		int right_down = x + PARTICLE_WIDTH + 1;
		
		if (right < PARTICLE_WIDTH*x && down < TOTAL_PARTICLES)	{
			
			// first triangle (top-left, bot-left, top-right)
			Vec3 normal1 = calculate_Normals(x, down, right);
			normal1 = normalized(normal1);
			// second triangle (bot-left, bot-right, top-right)
			Vec3 normal2 = calculate_Normals(right_down, down, right);
			normal2 = normalized(normal2);

			particles[x].normal.x += normal1.x;
			particles[x].normal.y += normal1.y;
			particles[x].normal.z += normal1.z;
		
			particles[down].normal.x += normal1.x + normal2.x;
			particles[down].normal.y += normal1.y + normal2.y;
			particles[down].normal.z += normal1.z + normal2.z;

			particles[right].normal.x += normal1.x + normal2.x;
			particles[right].normal.y += normal1.y + normal2.y;
			particles[right].normal.z += normal1.z + normal2.z;


			particles[right_down].normal.x += normal2.x;
			particles[right_down].normal.y += normal2.y;
			particles[right_down].normal.z += normal2.z;
		}
		

	}

}


void initialize_Particles()	{

	int x;
	int y;
	// will change to TOTAL_PARTICLES
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		int row = x/PARTICLE_WIDTH;
		int col = x - row * PARTICLE_WIDTH;
				
		// create points for rope/cloth
		Vec3 pos = { 0.25*col, -0.25*row, 0 };
		
		particles[x].position = pos;
		particles[x].old_position = pos;

		Vec3 zero = { 0,0,0 };	// zero vector
		particles[x].acceleration = zero;
		particles[x].normal = zero;

		particles[x].mass = MASS;
		particles[x].moveable = 1;

		// pin corners
		if ( x == 0 || x == PARTICLE_WIDTH - 1)	{
			particles[x].moveable = 0;
		}
		
	}
	// create normals for each point.
	initialize_Normals();
}


//******************
//* SPRING METHODS *
//******************

Spring springs[(PARTICLE_WIDTH - 1)*(PARTICLE_HEIGHT)]; // horizontal
Spring vert_springs[(PARTICLE_WIDTH)*(PARTICLE_HEIGHT - 1)]; // vertical
Spring diag1_springs[(PARTICLE_WIDTH - 1)*(PARTICLE_HEIGHT - 1)]; // top left to bottom right.
Spring diag2_springs[(PARTICLE_WIDTH - 1)*(PARTICLE_HEIGHT - 1)]; // top right to bottom left

void initialize_Springs()	{

	int x;
	int horiz = 0; //horizontal counter
	int vertic = 0; // vertical counter
	int diag11 = 0; // top-left to bot-right counter
	int diag22 = 0; // bot-left to top-right counter
	// rest lengths of springs should be all the same for springs in the same direction
	float rest_dist_X = particles[1].position.x - particles[0].position.x;
	float rest_dist_Y = particles[PARTICLE_WIDTH].position.y - particles[0].position.y;
	float rest_dist_diag = sqrt( (rest_dist_X*rest_dist_X) + (rest_dist_Y*rest_dist_Y) );

	// will change to TOTAL_PARTICLES
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		
		// at each point, look at each possible vertex
		int right = x + 1; // horizontal
		int down = x + PARTICLE_WIDTH; // vertical
		int right_down = x + PARTICLE_WIDTH + 1; //  bottom right 
		//printf(" x: %d, right: %d, down: %d, right_down: %d\n", x, right, down, right_down);
		
		// check if x isn't along the right edge...
		int right_check = ( x/PARTICLE_WIDTH) + 1;
		if (right < PARTICLE_WIDTH * right_check)	{
			// horizontal spring
			springs[horiz].point1 = x;
			springs[horiz].point2 = right;
			springs[horiz].restDistance = rest_dist_X;
			horiz++;
			// check diag spring. top left to bottom right.
			if (right_down < TOTAL_PARTICLES)	{
				//printf("Diag1\n");
				diag1_springs[diag11].point1 = x;
				diag1_springs[diag11].point2 = right_down;
				diag1_springs[diag11].restDistance = rest_dist_diag;
				
				diag11++;
				
			}
			
			// bottom left to top right
			if (down < TOTAL_PARTICLES)	{
				//printf("Diag2\n");
				diag2_springs[diag22].point1 = down;
				diag2_springs[diag22].point2 = right;
				diag2_springs[diag22].restDistance = rest_dist_diag;
				diag22++;
			}
		}

		// check vertical spring.
		if (down < TOTAL_PARTICLES)	{
			vert_springs[vertic].point1 = x;
			vert_springs[vertic].point2 = down;
			vert_springs[vertic].restDistance = rest_dist_Y;
			vertic++;
		}
	}
	
}

//********************
//* TIMESTEP METHODS *
//********************

// add gravity and wind
void AccumulateForces ()
{
    	int x;
	Vec3 gravity = { 0, GRAVITY, 0};
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		particles[x].acceleration = gravity;
	}

	
}

// update particles
void Verlet ()	{
	int x;
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		
		if (particles[x].moveable)	{

			Vec3 temp = particles[x].position;

			particles[x].position.x += ((temp.x - particles[x].old_position.x) * ( 1.0f - DAMPING) + 
					(particles[x].acceleration.x * TIMESTEP * TIMESTEP) );
			particles[x].position.y += ((temp.y - particles[x].old_position.y) * ( 1.0f - DAMPING) + 
					(particles[x].acceleration.y * TIMESTEP * TIMESTEP) );
			particles[x].position.z += ((temp.z - particles[x].old_position.z) * ( 1.0f - DAMPING) + 
					(particles[x].acceleration.z * TIMESTEP * TIMESTEP) );

			// set old position to temp
			particles[x].old_position = temp;

			// reset acceleration to 0
			Vec3 zero = { 0,0,0 };
			particles[x].acceleration = zero;
		}
	}
}

// Constrain Springs
void SatisfyConstraints()	{
	int iteration;
	int x;
	// horizontal.
	for (iteration = 0; iteration < CONSTRAINT_ITERATIONS; iteration++)	{
		for (x = 0; x < SPRING_COUNT; x++)	{

			int p1 = springs[x].point1;
			int p2 = springs[x].point2;
			
			Vec3 stretch = { particles[p2].position.x - particles[p1].position.x,
					 particles[p2].position.y - particles[p1].position.y,
					 particles[p2].position.z - particles[p1].position.z,
					};

			float current_distance = magnitude(stretch);
			float d = 0.5 * (1 - springs[x].restDistance/current_distance);
			
			Vec3 correction_vector = { stretch.x * d, stretch.y * d, stretch.z * d };

			if (particles[p1].moveable)	{
				particles[p1].position.x += correction_vector.x;
				particles[p1].position.y += correction_vector.y;
				particles[p1].position.z += correction_vector.z;
			}
			
			if (particles[p2].moveable)	{
				particles[p2].position.x -= correction_vector.x;
				particles[p2].position.y -= correction_vector.y;
				particles[p2].position.z -= correction_vector.z;
			}
		}
	}

	//vertical
	for (iteration = 0; iteration < CONSTRAINT_ITERATIONS; iteration++)	{
		for (x = 0; x < SPRING_COUNT; x++)	{

			int p1 = vert_springs[x].point1;
			int p2 = vert_springs[x].point2;
			
			Vec3 stretch = { particles[p2].position.x - particles[p1].position.x,
					 particles[p2].position.y - particles[p1].position.y,
					 particles[p2].position.z - particles[p1].position.z,
					};

			float current_distance = magnitude(stretch);
			float d = 0.5 * (1 - springs[x].restDistance/current_distance);
			
			Vec3 correction_vector = { stretch.x * d, stretch.y * d, stretch.z * d };

			if (particles[p1].moveable)	{
				particles[p1].position.x += correction_vector.x;
				particles[p1].position.y += correction_vector.y;
				particles[p1].position.z += correction_vector.z;
			}
			
			if (particles[p2].moveable)	{
				particles[p2].position.x -= correction_vector.x;
				particles[p2].position.y -= correction_vector.y;
				particles[p2].position.z -= correction_vector.z;
			}
		}
	}

	// Top-left to Bot-right
	for (iteration = 0; iteration < CONSTRAINT_ITERATIONS; iteration++)	{
		for (x = 0; x < SPRING_COUNT_DIAG; x++)	{
			
			int p1 = diag1_springs[x].point1;
			int p2 = diag1_springs[x].point2;
			

			Vec3 stretch = { particles[p2].position.x - particles[p1].position.x,
					 particles[p2].position.y - particles[p1].position.y,
					 particles[p2].position.z - particles[p1].position.z,
					};

			float current_distance = magnitude(stretch);
			float d = 0.5 * (1 - springs[x].restDistance/current_distance);
			
			Vec3 correction_vector = { stretch.x * d, stretch.y * d, stretch.z * d };

			if (particles[p1].moveable)	{
				particles[p1].position.x += correction_vector.x;
				particles[p1].position.y += correction_vector.y;
				particles[p1].position.z += correction_vector.z;
			}
			
			if (particles[p2].moveable)	{
				particles[p2].position.x -= correction_vector.x;
				particles[p2].position.y -= correction_vector.y;
				particles[p2].position.z -= correction_vector.z;
			}
		}
	}
	
	// Bot-left to Top-right
	for (iteration = 0; iteration < CONSTRAINT_ITERATIONS; iteration++)	{
		for (x = 0; x < SPRING_COUNT_DIAG; x++)	{
			
			int p1 = diag2_springs[x].point1;
			int p2 = diag2_springs[x].point2;
			

			Vec3 stretch = { particles[p2].position.x - particles[p1].position.x,
					 particles[p2].position.y - particles[p1].position.y,
					 particles[p2].position.z - particles[p1].position.z,
					};

			float current_distance = magnitude(stretch);
			float d = 0.5 * (1 - springs[x].restDistance/current_distance);
			
			Vec3 correction_vector = { stretch.x * d, stretch.y * d, stretch.z * d };

			if (particles[p1].moveable)	{
				particles[p1].position.x += correction_vector.x;
				particles[p1].position.y += correction_vector.y;
				particles[p1].position.z += correction_vector.z;
			}
			
			if (particles[p2].moveable)	{
				particles[p2].position.x -= correction_vector.x;
				particles[p2].position.y -= correction_vector.y;
				particles[p2].position.z -= correction_vector.z;
			}
		}
	}

}



void PrintValues()	{
	int x;
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		
		printf("particles[%d].position.x = %f\n", x, particles[x].position.x);
		printf("particles[%d].position.y = %f\n", x, particles[x].position.y);
		printf ("------------------------------------------------\n");
		
	}
        printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	
}

void TimeStep()	{
	AccumulateForces();
	Verlet();
	SatisfyConstraints();
	//PrintValues();
}


//*************
//* COLLISION *
//*************
// check if ball hits cloth.

Vec3 ball_position = {0,0,0};

void collision()	{
	
	int x;

	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		Vec3 check = { particles[x].position.x - ball_position.x,
			       particles[x].position.x - ball_position.x,
			       particles[x].position.x - ball_position.x };

		Vec3 norm = normalized(check);
		float length = magnitude(check);
		if ( length < BALL_RADIUS )	{
			particles[x].position.x += norm.x * (BALL_RADIUS - length);
			particles[x].position.y += norm.y * (BALL_RADIUS - length);
			particles[x].position.z += norm.z * (BALL_RADIUS - length);
		}
	}
	
}



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
}



int timer = 0;
int move = 0;
int direction = 0; // 0 for forward, 1 for backward
void display (void)
{
  
	
	if(!pause)	{
		TimeStep();
		timer++;
		//pause = 1;
	}
	
	if ( timer > 10) {
		// move ball.
		
		
		
		//collision();
	}
	
	
	ball_position.x = BALL_POSITION_X;
	ball_position.y = BALL_POSITION_Y;
	if ( move >= 0 ) 
		ball_position.z = BALL_POSITION_Z + move;
	else
		ball_position.z = BALL_POSITION_Z - move;
	
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

	
  glTranslatef (ball_position.x, ball_position.y, ball_position.z);
  glColor3f (1.0f, 1.0f, 0.0f);
  glutSolidSphere (BALL_RADIUS - 0.1, 50, 50); // draw the ball, but with a slightly lower radius, otherwise we could get ugly visual artifacts of rope penetrating the ball slightly
  glPopMatrix ();

 	
/*	
	// Draw the rope int its initial state
    	// Draw balls of the rope
    	int i;
	glEnable(GL_COLOR_MATERIAL);
   	glColor3f(1.0f, 1.0f, 1.0f);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
	glPointSize(5.0);
    	glBegin(GL_POINTS);
   
    	for (i = 0; i < TOTAL_PARTICLES; i++)    {   
		//glNormal3f(particles[i].normal.x, particles[i].normal.y, particles[i].normal.z);
        	glVertex3f(particles[i].position.x, particles[i].position.y, particles[i].position.z);
         
    	}
    	glEnd();
          
    	// Draw springs between the balls
    	int j;
	glEnable(GL_COLOR_MATERIAL);
   	glColor3f(1.0f, 1.0f, 1.0f);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(2.0);
    	glBegin(GL_LINES);
    	for (j = 0; j < SPRING_COUNT; j++)    {
		// horizontal spring
		int index1 = springs[j].point1;
		int index2 = springs[j].point2;
		
        	glVertex3f(particles[index1].position.x, particles[index1].position.y, particles[index1].position.z);
		glVertex3f(particles[index2].position.x, particles[index2].position.y, particles[index2].position.z);


		// vertical spring
		int index3 = vert_springs[j].point1;
		int index4 = vert_springs[j].point2;

		glVertex3f(particles[index3].position.x, particles[index3].position.y, particles[index3].position.z);
		glVertex3f(particles[index4].position.x, particles[index4].position.y, particles[index4].position.z);

    	}

	for (j = 0; j < SPRING_COUNT_DIAG; j++)	{
		// diagonals (topleft-botright) spring
		int index5 = diag1_springs[j].point1;
		int index6 = diag1_springs[j].point2;

		glVertex3f(particles[index5].position.x, particles[index5].position.y, particles[index5].position.z);
		glVertex3f(particles[index6].position.x, particles[index6].position.y, particles[index6].position.z);


		// diagonals (topleft-botright) spring
		int index7 = diag2_springs[j].point1;
		int index8 = diag2_springs[j].point2;

		glVertex3f(particles[index7].position.x, particles[index7].position.y, particles[index7].position.z);
		glVertex3f(particles[index8].position.x, particles[index8].position.y, particles[index8].position.z);

	}
	glEnd();
*/

	// Draw Cloth
	glEnable(GL_COLOR_MATERIAL);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	int x;
	for (x = 0; x < TOTAL_PARTICLES; x++)	{
		int right_check = x/PARTICLE_WIDTH + 1;

		int right = x + 1;
		int down = x + PARTICLE_WIDTH;
		int right_down = x + PARTICLE_WIDTH + 1;

		
		glColor3f(0.0,1.0,1.0);
		
		// draw cloth as a bunch of triangles
		if ( right < PARTICLE_WIDTH*right_check && down < TOTAL_PARTICLES)	{
			
			glNormal3f(particles[x].normal.x, particles[x].normal.y, particles[x].normal.z);
			glVertex3f(particles[x].position.x, particles[x].position.y, particles[x].position.z);
			glNormal3f(particles[down].normal.x, particles[down].normal.y, particles[down].normal.z);
			glVertex3f(particles[down].position.x, particles[down].position.y, particles[down].position.z);
			glNormal3f(particles[right].normal.x, particles[right].normal.y, particles[right].normal.z);
			glVertex3f(particles[right].position.x, particles[right].position.y, particles[right].position.z);

			glNormal3f(particles[right_down].normal.x, particles[right_down].normal.y, particles[right_down].normal.z);
			glVertex3f(particles[right_down].position.x, particles[right_down].position.y, particles[right_down].position.z);
			glNormal3f(particles[down].normal.x, particles[down].normal.y, particles[down].normal.z);
			glVertex3f(particles[down].position.x, particles[down].position.y, particles[down].position.z);
			glNormal3f(particles[right].normal.x, particles[right].normal.y, particles[right].normal.z);
			glVertex3f(particles[right].position.x, particles[right].position.y, particles[right].position.z);
		}
		
	}



	glEnd();



     	glFlush();
    	
     	

  glutSwapBuffers();
  glutPostRedisplay();
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
    case 27:    
      exit (0);
    break;  
    case 32:
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
      glutReshapeWindow (1280, 720 );
    break;
    default:
    break;
  }
}





int main (int argc, char *argv[]) 
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
  glutInitWindowSize (1280, 720 ); 
  glutCreateWindow ("Cloth simulator");
  init ();
  glutDisplayFunc (display);  
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutSpecialFunc (arrow_keys);

	// initialize particles and springs
	initialize_Particles();
	initialize_Springs();
	PrintValues();
  glutMainLoop ();
}

