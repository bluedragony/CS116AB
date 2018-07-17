// Michael Chen
// Marching Squares and metaballs by Jamie Wong helped with the implementation of the algorithm.
// Most of the code is taken from Robert Bruce's sample program.
// Marching Square Algorithm implemented by myself.

#include <math.h>
#include <time.h>
#include <stdlib.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
#elif __linux__
  #include <GL/gl.h>
  #include <GL/glut.h> 
#endif


#define X_RESOLUTION 800 
#define Y_RESOLUTION 600 
#define UNDEFINED -1
#define NORTH 0
#define NORTHEAST 1
#define EAST 2
#define SOUTHEAST 3
#define SOUTH 4
#define SOUTHWEST 5
#define WEST 6
#define NORTHWEST 7
#define TRUE 1
#define FALSE 0



typedef struct ball_color
{
  int r;
  int g;
  int b;
} ball_color;


typedef struct vec3
{
  int x;
  int y;
  int z;
  int direction;
} vec3;


typedef struct ball_type
{
  vec3 position;
  int radius;
  ball_color color;
} ball_type;



ball_type ball1;
ball_type ball2;
ball_type ball3;
ball_type ball4;
ball_type ball5;
ball_type ball6;
ball_type ball7;
ball_type ball8;

// added variables
// creates grid for marching squares
double grid[X_RESOLUTION/10 + 1][Y_RESOLUTION/10 + 1]; 


// added methods

void update_grid(void);
void marching_square_cases(void);
void draw_marching_square(void);

int ball_hit_wall (ball_type);
int select_ball_direction (ball_type);
ball_type move_ball (ball_type);
void display (void);
void reshape (int, int);
void keyboard (unsigned char, int, int);


//Update the grid vertex values

void update_grid(void)	{
	int x, y;
	for (x = 0; x < X_RESOLUTION/10 + 1; x ++)	{
		for (y = 0; y < Y_RESOLUTION/10 + 1; y ++)	{
			double temp = 0.0;
			double tempX = (double)x * 10; // used for 10 by 10 pixel squares
			double tempY = (double)y * 10; // used for 10 by 10 pixel squares

			temp = temp + (double)(ball1.radius * ball1.radius) / ( (tempX - (double)ball1.position.x)*(tempX - (double)ball1.position.x) + (tempY - (double)ball1.position.y)*(tempY - (double)ball1.position.y) );

			temp = temp + (double)(ball2.radius * ball2.radius) / ( (tempX - (double)ball2.position.x)*(tempX - (double)ball2.position.x) + (tempY - (double)ball2.position.y)*(tempY - (double)ball2.position.y) );

			temp = temp + (double)(ball3.radius * ball3.radius) / ( (tempX - (double)ball3.position.x)*(tempX - (double)ball3.position.x) + (tempY - (double)ball3.position.y)*(tempY - (double)ball3.position.y) );

			temp = temp + (double)(ball4.radius * ball4.radius) / ( (tempX - (double)ball4.position.x)*(tempX - (double)ball4.position.x) + (tempY - (double)ball4.position.y)*(tempY - (double)ball4.position.y) );

			temp = temp + (double)(ball5.radius * ball5.radius) / ( (tempX - (double)ball5.position.x)*(tempX - (double)ball5.position.x) + (tempY - (double)ball5.position.y)*(tempY - (double)ball5.position.y) );

			temp = temp + (double)(ball6.radius * ball6.radius) / ( (tempX - (double)ball6.position.x)*(tempX - (double)ball6.position.x) + (tempY - (double)ball6.position.y)*(tempY - (double)ball6.position.y) );

			temp = temp + (double)(ball7.radius * ball1.radius) / ( (tempX - (double)ball7.position.x)*(tempX - (double)ball7.position.x) + (tempY - (double)ball7.position.y)*(tempY - (double)ball7.position.y) );

			temp = temp + (double)(ball8.radius * ball1.radius) / ( (tempX - (double)ball8.position.x)*(tempX - (double)ball8.position.x) + (tempY - (double)ball8.position.y)*(tempY - (double)ball8.position.y) );

			grid [x][y] = temp;
		}
	}
	marching_square_cases();

}

// Cases of Marching Squares
int cases[X_RESOLUTION/10 + 1][Y_RESOLUTION/10 + 1]; // determine each squares case.

void marching_square_cases(void)	{
	int NW; // northwest corner
	int NE; // northeast corner
	int SW; // southwest corner
	int SE; // southeast corner
	
	int i,j;
	for (i = 0; i <= X_RESOLUTION/10; i++)	{
		for (j = 0; j <= Y_RESOLUTION/10; j++)	{
			NW = grid[i][j];
			NE = grid[i+1][j];
			SW = grid[i][j+1];
			SE = grid[i+1][j+1];
			
			// case 0;
			if (NW == 0 && NE == 0 && SE == 0 && SW == 0)
				cases[i][j] = 0;
			// case 1;
			if (NW == 0 && NE == 0 && SE == 0 && SW >= 1)
				cases[i][j] = 1;
			// case 2;
			if (NW == 0 && NE == 0 && SE >= 1 && SW == 0)
				cases[i][j] = 2;
			// case 3;
			if (NW == 0 && NE == 0 && SE >= 1 && SW >= 1)
				cases[i][j] = 3;
			// case 4;
			if (NW == 0 && NE >= 1 && SE == 0 && SW == 0)
				cases[i][j] = 4;
			// case 5;
			if (NW == 0 && NE >= 1 && SE == 0 && SW >= 1)
				cases[i][j] = 5;
			// case 6;
			if (NW == 0 && NE >= 1 && SE >= 1 && SW == 0)
				cases[i][j] = 6;
			// case 7;
			if (NW == 0 && NE >= 1 && SE >= 1 && SW >= 1)
				cases[i][j] = 7;
			// case 8;
			if (NW >= 1 && NE == 0 && SE == 0 && SW == 0)
				cases[i][j] = 8;
			// case 9;
			if (NW >= 1 && NE == 0 && SE == 0 && SW >= 1)
				cases[i][j] = 9;
			// case 10;
			if (NW >= 1 && NE == 0 && SE >= 1 && SW == 0)
				cases[i][j] = 10;
			// case 11;
			if (NW >= 1 && NE == 0 && SE >= 1 && SW >= 1)
				cases[i][j] = 11;
			// case 12;
			if (NW >= 1 && NE >= 1 && SE == 0 && SW == 0)
				cases[i][j] = 12;
			// case 13;
			if (NW >= 1 && NE >= 1 && SE == 0 && SW >= 1)
				cases[i][j] = 13;
			// case 14;
			if (NW >= 1 && NE >= 1 && SE >= 1 && SW == 0)
				cases[i][j] = 14;
			// case 15;
			if (NW >= 1 && NE >= 1 && SE >= 1 && SW >= 1)
				cases[i][j] = 15;
		} 
	}
	draw_marching_square();
}



// drawing marching squares

void draw_marching_square (void)	{

	glColor3ub ( 255, 0, 0 );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
	int x,y;
	for ( x = 0; x < X_RESOLUTION/10; x ++)	{
		for ( y = 0; y < Y_RESOLUTION/10; y++)	{
			// the corners of each square.
			double BOT_X = (float)x * 10; // 10x10 square
			double BOT_Y = (float)y * 10;
			double TOP_X = (float)(x + 1)*10;
			double TOP_Y = (float)(y + 1)*10;
			
			//Need some midpoints because of the cases for marching square
			double MID_X = ((float)x+0.5)*10;
			double MID_Y = ((float)y+0.5)*10;			

			int draw_case = cases[x][y]; // draws the corresponding case

			glBegin(GL_POLYGON);

			if (draw_case == 0)	{
				//draw nothing.
			}
			else if (draw_case == 1)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
				glVertex3f(BOT_X,MID_Y,0); 
			}
			else if (draw_case == 2)	{
				glVertex3f(MID_X,TOP_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
			}
			else if (draw_case == 3)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
			}
			else if (draw_case == 4)	{
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
			}
			else if (draw_case == 5)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
			}
			else if (draw_case == 6)	{
				glVertex3f(MID_X,TOP_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
			}
			else if (draw_case == 7)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
			}
			else if (draw_case == 8)	{
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
				glVertex3f(MID_X,BOT_Y,0);		
			}
			else if (draw_case == 9)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
			}
			else if (draw_case == 10)	{
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
			}
			else if (draw_case == 11)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(MID_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
			}
			else if (draw_case == 12)	{
				glVertex3f(BOT_X,MID_Y,0);
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
			}
			else if (draw_case == 13)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,MID_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
			}
			else if (draw_case == 14)	{
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
				glVertex3f(MID_X,TOP_Y,0);
				glVertex3f(BOT_X,MID_Y,0);
			}
			else if (draw_case == 15)	{
				glVertex3f(BOT_X,TOP_Y,0);
				glVertex3f(BOT_X,BOT_Y,0);
				glVertex3f(TOP_X,BOT_Y,0);
				glVertex3f(TOP_X,TOP_Y,0);
			}
			glEnd();
		}
	}

}




int ball_hit_wall (ball_type ball)
{
  int ball_hit_wall;

  ball_hit_wall = FALSE;
  switch (ball.position.direction)
  {
    case NORTH:
      if ((ball.position.y - ball.radius) <= 0)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case NORTHEAST:
      if (((ball.position.y - ball.radius) <= 0) || ((ball.position.x + ball.radius) >= X_RESOLUTION))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case EAST:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTHEAST:
      if (((ball.position.y + ball.radius) >= Y_RESOLUTION) || ((ball.position.x + ball.radius) >= X_RESOLUTION))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTH:
      if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case SOUTHWEST:
      if (((ball.position.y + ball.radius) >= Y_RESOLUTION) || ((ball.position.x - ball.radius) <= 0))
      {
        ball_hit_wall = TRUE;
      }
    break;
    case WEST:
      if ((ball.position.x - ball.radius) <= 0)
      {
        ball_hit_wall = TRUE;
      }
    break;
    case NORTHWEST:
      if (((ball.position.y - ball.radius) <= 0) || ((ball.position.x - ball.radius) <= 0))
      {
        ball_hit_wall = TRUE;
      }
    break;
    default:
    break;
  }
  return ball_hit_wall;
}





int select_ball_direction (ball_type ball)
{
  int new_ball_direction, random_direction;
  float new_move_north, new_move_northeast, new_move_east, new_move_southeast, new_move_south, new_move_southwest, new_move_west, new_move_northwest;
  float random_number, lower_bound, upper_bound;

  switch (ball.position.direction)
  {
    case NORTH:
      if (((ball.position.x - ball.radius) <= 0) && ((ball.position.y - ball.radius) <= 0))
      {
        new_ball_direction = SOUTHEAST;
      }
      else
      {
        if (((ball.position.x + ball.radius) >= X_RESOLUTION) && ((ball.position.y - ball.radius) <= 0))
        {
          new_ball_direction = SOUTHWEST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = SOUTHEAST;
            break;
            case 1:
              new_ball_direction = SOUTHWEST;
            break;
            default:
              new_ball_direction = SOUTHEAST;
            break;
          }
        }
      }     
    break;
    case NORTHEAST:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION)
      {
        if ((ball.position.y - ball.radius) <= 0)
        {
          new_ball_direction = SOUTHWEST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHWEST;
            break;
            case 1:
              new_ball_direction = WEST;
            break;
            default:
              new_ball_direction = NORTHWEST;
            break;
          }
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = SOUTHEAST;
          break;
          case 1:
            new_ball_direction = EAST;
          break;
          default:
            new_ball_direction = SOUTHEAST;
          break;
        }
      }
    break;
    case EAST:
      if ((ball.position.y - ball.radius) <= 0)
      {
        new_ball_direction = SOUTHWEST;
      }
      else
      {
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHWEST; 
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHWEST; 
            break;
            case 1:
              new_ball_direction = SOUTHWEST;
            break;
            default:
              new_ball_direction = NORTHWEST;
            break;
          }
        }
      }
    break;
    case SOUTHEAST:
      if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
      {
        if ((ball.position.x + ball.radius) >= X_RESOLUTION)
        {
          new_ball_direction = NORTHWEST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTH;
            break;
            case 1:
              new_ball_direction = NORTHEAST;
            break;
            case 2:
              new_ball_direction = EAST;
            break;
            default:
              new_ball_direction = NORTH;
            break;
          }
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = SOUTH;
          break;
          case 1:
            new_ball_direction = SOUTHWEST;
          break;
          case 2:
            new_ball_direction = WEST;
          break;
          default:
            new_ball_direction = SOUTH;
          break;
        }
      }
    break;
    case SOUTH:
      if ((ball.position.x + ball.radius) >= X_RESOLUTION)
      {
        new_ball_direction = NORTHWEST;
      }
      else
      {
        new_ball_direction = NORTHEAST;
      }
    break;
    case SOUTHWEST:
      if ((ball.position.x - ball.radius) <= 0)
      { 
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHEAST;
        }
        else
        {
          new_ball_direction = SOUTHEAST;
        }
      }
      else
      {
        random_number = (float) random() / (float) RAND_MAX * 2.0f;
        random_direction = (int) random_number;
        switch (random_direction)
        {
          case 0:
            new_ball_direction = NORTH;
          break;
          case 1:
            new_ball_direction = NORTHWEST;
          break;
          case 2:
            new_ball_direction = WEST;
          break;
          default:
            new_ball_direction = NORTH;
          break;
        }
      }
    break;
    case WEST:
      if ((ball.position.y - ball.radius) <= 0)
      {
        new_ball_direction = SOUTHEAST;
      }
      else
      {
        if ((ball.position.y + ball.radius) >= Y_RESOLUTION)
        {
          new_ball_direction = NORTHEAST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHEAST;
            break;
            case 1:
              new_ball_direction = SOUTHEAST;
            break;
            default:
              new_ball_direction = NORTHEAST;
            break;
          }
        }
      }
    break;
    case NORTHWEST:
      if ((ball.position.x - ball.radius) <= 0)
      { 
        if ((ball.position.y - ball.radius) <= 0)
        {
          new_ball_direction = SOUTHEAST;
        }
        else
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = NORTHEAST;
            break;
            case 1:
              new_ball_direction = EAST;
            break;
            default:
              new_ball_direction = NORTHEAST;
            break;
          }
        }
      }
      else
      {
        if ((ball.position.y - ball.radius) <= 0)
        {
          random_number = (float) random() / (float) RAND_MAX * 2.0f;
          random_direction = (int) random_number;
          switch (random_direction)
          {
            case 0:
              new_ball_direction = SOUTHWEST;
            break;
            case 1:
              new_ball_direction = SOUTH;
            break;
            case 2:
              new_ball_direction = WEST;
            break;
            default:
              new_ball_direction = SOUTHWEST;
            break;
          }
        }
      }
    break;
    default:
    break;
  }
  ball.position.direction = new_ball_direction;
  return ball.position.direction;
}





ball_type move_ball (ball_type ball)
{
  ball_type new_ball;

  new_ball = ball;
  switch (ball.position.direction)
  {
    case NORTH:
      new_ball.position.y--;
    break;
    case NORTHEAST:
      new_ball.position.x++;
      new_ball.position.y--;
    break;
    case EAST:
      new_ball.position.x++;
    break;
    case SOUTHEAST:
      new_ball.position.x++;
      new_ball.position.y++;
    break;
    case SOUTH:
      new_ball.position.y++;
    break;
    case SOUTHWEST:
      new_ball.position.x--;
      new_ball.position.y++;
    break;
    case WEST:
      new_ball.position.x--;
    break;
    case NORTHWEST:
      new_ball.position.x--;
      new_ball.position.y--;
    break;
    default:
    break;
  }
  return new_ball;
}




void draw_ball (ball_type ball)
{
  float theta, circle_iterations = 24.0;

  glColor3ub (ball.color.r, ball.color.g, ball.color.b);
  glPolygonMode (GL_FRONT, GL_FILL);
  glBegin (GL_POLYGON);
  glVertex3f (ball.position.x + ball.radius, ball.position.y, 0.0f);
  for (theta = 0; theta < 2 * M_PI; theta += M_PI / circle_iterations)
  {
    glVertex3f (ball.position.x + cos(theta) * ball.radius, ball.position.y + sin(theta) * ball.radius, 0.0f);
  }
  glEnd();
}



void display (void)
{
// **********
// * Ball 1 *
// **********
  if (ball_hit_wall(ball1))
  {
    ball1.position.direction = select_ball_direction (ball1);
  }
  ball1 = move_ball (ball1);
// **********
// * Ball 2 *
// **********
  if (ball_hit_wall(ball2))
  {
    ball2.position.direction = select_ball_direction (ball2);
  }
  ball2 = move_ball (ball2);
// **********
// * Ball 3 *
// **********
  if (ball_hit_wall(ball3))
  {
    ball3.position.direction = select_ball_direction (ball3);
  }
  ball3 = move_ball (ball3);
// **********
// * Ball 4 *
// **********
  if (ball_hit_wall(ball4))
  {
    ball4.position.direction = select_ball_direction (ball4);
  }
  ball4 = move_ball (ball4);
// **********
// * Ball 5 *
// **********
  if (ball_hit_wall(ball5))
  {
    ball5.position.direction = select_ball_direction (ball5);
  }
  ball5 = move_ball (ball5);
// **********
// * Ball 6 *
// **********
  if (ball_hit_wall(ball6))
  {
    ball6.position.direction = select_ball_direction (ball6);
  }
  ball6 = move_ball (ball6);
// **********
// * Ball 7 *
// **********
  if (ball_hit_wall(ball7))
  {
    ball7.position.direction = select_ball_direction (ball7);
  }
  ball7 = move_ball (ball7);
// **********
// * Ball 8 *
// **********
  if (ball_hit_wall(ball8))
  {
    ball8.position.direction = select_ball_direction (ball8);
  }
  ball8 = move_ball (ball8);


//
// clear the screen to black
  glColor3ub (0, 0, 0);
  glPolygonMode (GL_FRONT, GL_FILL);
  glRecti (0, 0, X_RESOLUTION, Y_RESOLUTION);
//
//


	update_grid();
/*
  draw_ball (ball1);
  draw_ball (ball2);
  draw_ball (ball3);
  draw_ball (ball4);
  draw_ball (ball5);
  draw_ball (ball6);
  draw_ball (ball7);
  draw_ball (ball8);
*/
	
  glutSwapBuffers();
  glutPostRedisplay();
}





void reshape (int w, int h)  
{
  glViewport (0, 0, w, h);
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();  
  if (h == 0)  
  {
    glOrtho (0.0, X_RESOLUTION, 1, 0, -1.0, 1.0);
  }
  else
  {
    glOrtho (0.0, X_RESOLUTION, Y_RESOLUTION, 0, -1.0, 1.0);
  }
  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity(); 
}





void keyboard (unsigned char key, int x, int y) 
{
  switch (key) 
  {
    case 27:    
      exit (0);
    break;  
    default: 
    break;
  }
}






int main (int argc, char *argv[]) 
{


// **************************
// * ball 1 characteristics *
// **************************
  ball1.position.x = X_RESOLUTION / 2;
  ball1.position.y = Y_RESOLUTION / 2,
  ball1.position.z = 0;
  ball1.position.direction = NORTH;
  ball1.radius = 50;
  ball1.color.r = 255;
  ball1.color.g = 255;
  ball1.color.b = 0;
// **************************
// * ball 2 characteristics *
// **************************
  ball2.position.x = X_RESOLUTION / 2;
  ball2.position.y = Y_RESOLUTION / 2;
  ball2.position.z = 0;
  ball2.position.direction = EAST;
  ball2.radius = 50;
  ball2.color.r = 255;
  ball2.color.g = 255;
  ball2.color.b = 0;
// **************************
// * ball 3 characteristics *
// **************************
  ball3.position.x = X_RESOLUTION / 2;
  ball3.position.y = Y_RESOLUTION / 2;
  ball3.position.z = 0;
  ball3.position.direction = SOUTH;
  ball3.radius = 50;
  ball3.color.r = 255;
  ball3.color.g = 255;
  ball3.color.b = 0;
// **************************
// * ball 4 characteristics *
// **************************
  ball4.position.x = X_RESOLUTION / 2;
  ball4.position.y = Y_RESOLUTION / 2;
  ball4.position.z = 0;
  ball4.position.direction = WEST;
  ball4.radius = 50;
  ball4.color.r = 255;
  ball4.color.g = 255;
  ball4.color.b = 0;
// **************************
// * ball 5 characteristics *
// **************************
  ball5.position.x = X_RESOLUTION / 2;
  ball5.position.y = Y_RESOLUTION / 2;
  ball5.position.z = 0;
  ball5.position.direction = NORTHEAST;
  ball5.radius = 50;
  ball5.color.r = 255;
  ball5.color.g = 255;
  ball5.color.b = 0;
// **************************
// * ball 6 characteristics *
// **************************
  ball6.position.x = X_RESOLUTION / 2;
  ball6.position.y = Y_RESOLUTION / 2;
  ball6.position.z = 0;
  ball6.position.direction = NORTHWEST;
  ball6.radius = 50;
  ball6.color.r = 255;
  ball6.color.g = 255;
  ball6.color.b = 0;
// **************************
// * ball 7 characteristics *
// **************************
  ball7.position.x = X_RESOLUTION / 2;
  ball7.position.y = Y_RESOLUTION / 2;
  ball7.position.z = 0;
  ball7.position.direction = SOUTHEAST;
  ball7.radius = 50;
  ball7.color.r = 255;
  ball7.color.g = 255;
  ball7.color.b = 0;
// **************************
// * ball 8 characteristics *
// **************************
  ball8.position.x = X_RESOLUTION / 2;
  ball8.position.y = Y_RESOLUTION / 2;
  ball8.position.z = 0;
  ball8.position.direction = SOUTHEAST;
  ball8.radius = 50;
  ball8.color.r = 255;
  ball8.color.g = 255;
  ball8.color.b = 0;
//
// seed the random number generator
  srandom (time(0));
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); 
  glutInitWindowSize (X_RESOLUTION, Y_RESOLUTION);
  glutCreateWindow ("Marching Squares");
  glutDisplayFunc (display);  
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutMainLoop();
}

