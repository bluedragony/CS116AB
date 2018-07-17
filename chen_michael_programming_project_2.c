// Michael Chen
// most code taken from Rob Bruce's skeleton program.
// Implemented stuff that happens after pressing space bar.
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif
 
 
int   cube_exploded = 0;
float angle = 0.0; // camera rotation angle
 
float velocity = 0.9; // velocity
float t = 0; // start time 
 
float x,y,z;
 
// color
float red = 1.0f;
float green = 1.0f;
float blue = 1.0f;
 
 
// vector coords
typedef struct vec3{
        float x;
        float y;
        float z;
    int direction;
} vec3;
 
typedef struct Particle{
    vec3 position;
} Particle;
 
#define NumParticles 20000
Particle p[NumParticles]; // position vector
Particle pV[NumParticles]; // velocity vector
 
// Light sources
GLfloat  light0Amb[4] =  { 1.0, 0.6, 0.2, 1.0 };
GLfloat  light0Dif[4] =  { 1.0, 0.6, 0.2, 1.0 };
GLfloat  light0Spec[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat  light0Pos[4] =  { 0.0, 0.0, 0.0, 1.0 };
GLfloat  light1Amb[4] =  { 0.0, 0.0, 0.0, 1.0 };
GLfloat  light1Dif[4] =  { 1.0, 1.0, 1.0, 1.0 };
GLfloat  light1Spec[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat  light1Pos[4] =  { 0.0, 5.0, 5.0, 0.0 };
 
void init(void);
void display (void);
void keyboard (unsigned char, int, int);
void reshape (int, int);
void idle (void);
void explode_cube (void);
 
void init (void)    {
    // give directions for velocity
    int i;
    for( i = 0; i < NumParticles; i++){
        // give directions for velocity ranges from -1 to 1 and all in between.
        x = (rand() / (double) (RAND_MAX) ) * 2 - 1;
        y = (rand() / (double) (RAND_MAX) ) * 2 - 1;
        z = (rand() / (double) (RAND_MAX) ) * 2 - 1;
        pV[i].position.x = x;
        pV[i].position.y = y;
        pV[i].position.z = z;
         
        // give initial displacements as zeros
        p[i].position.x = 0;
        p[i].position.y = 0;
        p[i].position.z = 0;
    }
}
 
void display (void)
{
 
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();
//
// Place the camera
  glTranslatef (0.0, 0.0, -10.0);
  glRotatef (angle, 0.0, 1.0, 0.0);
     
 
    // If no explosion, draw cube
    if (!cube_exploded)
    {
        glEnable (GL_LIGHTING);
        glDisable (GL_LIGHT0);
        glEnable (GL_DEPTH_TEST);
        glutSolidCube (1.0);
    }
    else{
    	glEnable(GL_COLOR_MATERIAL);
        glColor3f(red, green, blue);
    	glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
	glPointSize(5.0);
        glBegin(GL_POINTS);
	
    	int i;
        for(i = 0; i < NumParticles; i++){
         
        glVertex3f(p[i].position.x,p[i].position.y,p[i].position.z);
        /*if (p[i].position.direction == 0) 
            glVertex3f(p[i].position.x,p[i].position.y,p[i].position.z);
        if (p[i].position.direction == 1)   
            glVertex3f(p[i].position.x,p[i].position.y,p[i].position.z);
        if (p[i].position.direction == 2)   
            glVertex3f(p[i].position.x,p[i].position.y,p[i].position.z);
        if (p[i].position.direction == 3)   
            glVertex3f(p[i].position.x,-p[i].position.y,p[i].position.z);
        if (p[i].position.direction == 4)   
            glVertex3f(p[i].position.x,-p[i].position.y,p[i].position.z); 
        if (p[i].position.direction == 5)   
            glVertex3f(-p[i].position.x,-p[i].position.y,p[i].position.z);       
        if (p[i].position.direction == 6)   
            glVertex3f(-p[i].position.x,p[i].position.y,p[i].position.z);
        if (p[i].position.direction == 7)   
            glVertex3f(-p[i].position.x,p[i].position.y,p[i].position.z);
        */
 
    }
     
        glEnd();
    }
    glutSwapBuffers ();
     
}
 
void idle (void)
{
     
    if( cube_exploded == 0 )
            angle += 0.3;  /* Always continue to rotate the camera */
     
    if(cube_exploded == 1){
     
        red -= 0.003;    
    green -= 0.006;
    blue -= 0.001;
         
        
    int i = 0;
    //update displacements.
        while(i != NumParticles){
        p[i].position.x = pV[i].position.x * velocity * t;
        p[i].position.y = pV[i].position.y * velocity * t;
        p[i].position.z = pV[i].position.z * velocity * t;
 
                i++;
        }
     t += 0.02;
    }
    glutPostRedisplay ();
}
 
void keyboard (unsigned char key, int x, int y)
{
    switch (key)
    {
        case ' ':           
            explode_cube();
            break;
        case 27:
            exit (0);
            break;
    }
}
 
void reshape (int w, int h)
{
 
    glViewport (0.0, 0.0, (GLfloat) w, (GLfloat) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (45.0, (GLfloat) w / (GLfloat) h, 0.1, 100.0);
    glMatrixMode (GL_MODELVIEW);
}
 
void explode_cube(void)
{
    cube_exploded = 1;
 
}
 
int main (int argc, char *argv[])
{
    glutInit (&argc, argv);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize (1024, 800);
    glutInitDisplayMode (GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow ("Particle explosion");
    glutKeyboardFunc (keyboard);
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutReshapeFunc (reshape);
    glEnable (GL_LIGHT0);
    glEnable (GL_LIGHT1);
    glLightfv (GL_LIGHT0, GL_AMBIENT, light0Amb);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light0Dif);
    glLightfv (GL_LIGHT0, GL_SPECULAR, light0Spec);
    glLightfv (GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv (GL_LIGHT1, GL_AMBIENT, light1Amb);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light1Dif);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light1Spec);
    glLightfv (GL_LIGHT1, GL_POSITION, light1Pos);
    glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable (GL_NORMALIZE);
    init();
     
    glutMainLoop ();
    return 0;
}
