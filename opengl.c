#include <GL/glut.h>
#include "constants.h"
#include "functions.h"
#include "header.h"

//------------------------------------------------[OpenGL variables]-------------------------------------------------------------//
void startOpenGl();
void display();
void init();
void reshape(int w, int h);
void timer(int value);
void rectangle(int x1, int y1, int x2, int y2, float r, float g, float b);
void drawText_size(int centerX, int centerY, char *str, int size);
void drawLine(int centerX1, int centerY1, int centerX2, int centerY2);
void writeIntegerAtPosition(int number, float x, float y);
void drawText(float x, float y, const char *text , float r, float g, float b) ;
void drawSquare(float x, float y, float size, float borderSize, float r, float g, float b);

float move_x = 300.0; 		// Initial x position of the rectangle
float move_speed = 2.0; 	// Speed of movement
float move_y = 300.0; 		// Initial x position of the rectangle
// define square struct
typedef struct {
    float x;
    float y;
    float size;
    float borderSize;
    float r;
    float g;
    float b;
    
} Square;
// Production line struct
typedef struct{
    int id;
    int accepted;
    int discarded;
    int produced;
} ProductionLine;


// define array for packaging
int store_accepted[100];
//--------------------------------------------------------------------
int store_discarded[100];
int store_produced[100];
// array of production lines
ProductionLine production_lines[100];

//-------------------------------------------------[    Main    ]----------------------------------------------------------------//

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	startOpenGl();
    return 0;
}

//------------------------------------------------[OpenGL functions]-------------------------------------------------------------//

void startOpenGl(){
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(900, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Medicine Production");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    init();
    glutMainLoop();
}

//--------------------------------------------------------------------

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 600, 0, 600);
	glMatrixMode(GL_MODELVIEW);
}

//--------------------------------------------------------------------


void init()
{
	glClearColor(1, 1, 1, 1);
}

//--------------------------------------------------------------------

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    readFile("args.txt");

    for (int i = 0 ; i< NUMBER_OF_PRODUCTION_LINES ; i++){
        // display text
         char text[100]; // Buffer to hold the formatted string
        snprintf(text, sizeof(text), "Production Line %d", i);
        drawText(50, 450 - 2*i * 100, text, 0, 0, 0);
       drawText(50 , 450 - 2*i * 100-20, "Number of accepted", 0, 0, 0 );
        writeIntegerAtPosition(store_accepted[i],210,450-2*i*100-20);
        drawText(50 , 450 - 2*i * 100-50, "Number of discarded", 0, 0, 0 );
         writeIntegerAtPosition(store_discarded[i],210,450-2*i*100-50);
          drawText(50 , 450 - 2*i * 100-70, "Total # Of produced", 0, 0, 0 );
         writeIntegerAtPosition(store_produced[i],210,450- 2*i*100-70);
        // draw text inspecsion and packaging employees
        drawText(250, 450 - 2*i * 100, "produced", 0, 0, 0);
        // draw square 
        drawSquare(250, 450 - 2*i * 100 -40 , 20, 2,1, 1, 1); // Second rectangle
        drawText(350, 450 -2* i * 100, "inspecsion", 0, 0, 0);
        // draw square
        drawSquare(350, 450 - 2*i * 100 -40 , 20, 2,1, 1, 1); // Second rectangle

        drawText(450, 450 - 2*i * 100, "packaging", 0, 0, 0);
        // draw square
        drawSquare(450, 450 -  2*i * 100 -40 , 20, 2, 1, 1, 1); // Second rectangle
        // write the number of accepted
    }

  

    char *msg= receive_a_message(); // message can be accepted , discarded , produce with production line id
  
    if(msg !=NULL){
    char* token = strtok(msg, " ");
    char* number = strtok(NULL, " ");
    // covert number to integer
    int id = atoi(number) -1;
    //  production_lines[id].id = id;
    if (token != NULL && strcmp(token, "accepted") == 0) {
      
        store_accepted[id]++;
        //store in the array of struct
        // production_lines[id].accepted++;
      

	}else if(token != NULL && strcmp(token, "discarded") == 0){

    store_discarded[id]++;
    //store in the array of struct
    // production_lines[id].discarded++;

	 
	  }else if(token != NULL && strcmp(token, "produce") == 0){

        store_produced[id]++;
        //store in the array of struct
        // production_lines[id].produced++;

	 
	  }

    }
    //  drawText(10, 100, "Production Line 1", 0, 0, 0);
    //  drawText(120, 100, "Number of accepted", 0, 0, 0);
    //   writeIntegerAtPosition(store_accepted[0],250,100);
    //     drawText(10, 50, "Number of discarded", 0, 0, 0);
    //   writeIntegerAtPosition(store_discarded[0],150,50);
    //     drawText(10, 20, "Total # Of produced", 0, 0, 0);
    //   writeIntegerAtPosition(store_produced[0],150,20);

    //      drawText(350, 100, "Production Line 2 Number of accepted", 0, 0, 0);
    // //  drawText(400, 100, "Number of accepted", 0, 0, 0);
    //   writeIntegerAtPosition(store_accepted[1],550,100);
    //     drawText(350, 50, "Number of discarded", 0, 0, 0);
    //   writeIntegerAtPosition(store_discarded[1],550,50);
    //     drawText(350, 20, "Total # Of produced", 0, 0, 0);
    //   writeIntegerAtPosition(store_produced[1],550,20);
    
      

    glFlush();
    glutPostRedisplay();  // Ensure continuous update

}


//--------------------------------------------------------------------

void timer(int value) {
    glutPostRedisplay();  // Mark the current window as needing to be redisplayed
    glutTimerFunc(16, timer, 0);  // Approx 60 FPS
}
//--------------------------------------------------------------------

void rectangle(int x1, int y1, int x2, int y2, float r, float g, float b)
{
	glBegin(GL_POLYGON);
	glColor3f(r, g, b);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}

//--------------------------------------------------------------------

void drawSquare(float x, float y, float size, float borderSize, float r, float g, float b) {
    // Draw the border square (black)
    glColor3f(0, 0, 0); // Set the color of the border (black)
    glBegin(GL_QUADS); // Draw a filled quadrilateral
        glVertex2f(x - borderSize, y + borderSize);
        glVertex2f(x + size + borderSize, y + borderSize);
        glVertex2f(x + size + borderSize, y - size - borderSize);
        glVertex2f(x - borderSize, y - size - borderSize);
    glEnd();

    // Draw the inner square (white)
    glColor3f(r, g, b); // Set the color of the inner square (white)
    glBegin(GL_QUADS); // Draw a filled quadrilateral
        glVertex2f(x, y);
        glVertex2f(x + size, y);
        glVertex2f(x + size, y - size);
        glVertex2f(x, y - size);
    glEnd();
}


//--------------------------------------------------------------------

void drawText_size(int centerX, int centerY, char *str, int size)
{
	glRasterPos2i(centerX, centerY); // Position for Square 1 text
	const char *text1 = str;

	if (size == 18)
	{
		glColor3f(1, 1, 1);
		for (int i = 0; i < strlen(text1); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text1[i]);
	}
	else if (size == 10)
	{
		glColor3f(0, 0, 0);
		for (int i = 0; i < strlen(text1); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, text1[i]);
	}
	else if (size == 15)
	{
		glColor3f(0, 0, 0);
		for (int i = 0; i < strlen(text1); i++)
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text1[i]);
	}
}

//--------------------------------------------------------------------

void drawLine(int centerX1, int centerY1, int centerX2, int centerY2)
{
	glColor3f(0, 0, 0); // Set color to black
	glBegin(GL_LINES);
	glVertex2i(centerX1, centerY1);
	glVertex2i(centerX2, centerY2);
	glEnd();
}

//--------------------------------------------------------------------

void writeIntegerAtPosition(int number, float x, float y)
{
	glRasterPos2f(x, y);
	char str[10];
	sprintf(str, "%d", number);
	for (int i = 0; str[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
}

//--------------------------------------------------------------------
void drawText(float x, float y, const char *text , float r, float g, float b) {
	glColor3f(r, g, b); // Set the color of the text
	glRasterPos2f(x, y); // Set the position for the text
	while (*text) {
		// font size is 18
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *text); // Render the character using GLUT's built-in font
		++text;
	}
}
