/**
 *  Simulator for Quanser's Coupled Tanks GUI
 *  Copyright (C) 2015,2016, Augusto Damasceno
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <pthread.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __unix__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#define DEBUG_MODE

/* Tanks Levels */
float tank0, tank1;

int getR;
int initOK;

/* Update Tanks Levels Info */
void * getData(void * in)
{
    int endpointfd;
    int dataSize;
    char buffer[64];
    struct sockaddr_in addr;
    socklen_t addrSize;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(20081);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(addr.sin_zero, '\0', sizeof addr.sin_zero);  

    addrSize = sizeof addr;

    endpointfd = socket(PF_INET, SOCK_STREAM, 0);
    if (endpointfd < 0)
    {
#ifdef DEBUG_MODE
        printf("ERROR opening socket.\n");
#endif
        getR = -1;
        pthread_exit(&getR);
    }

#ifdef DEBUG_MODE
    printf("Socket opened.\n");
#endif

    if (connect(endpointfd, (struct sockaddr *) &addr, addrSize) < 0)
    {
#ifdef DEBUG_MODE
        printf("ERROR connecting socket.\n");
#endif
        getR = -2;
        pthread_exit(&getR);
    }

#ifdef DEBUG_MODE
    printf("Socket connected.\n");
#endif

    char c = '0';
    int counter;
    int channel = 0;
    while(1)
    {
        if(channel)
        {
            dataSize = sprintf(buffer,"READ 1\n");
        }
        else
        {
            dataSize = sprintf(buffer,"READ 0\n");
        }

        sendto(endpointfd,buffer,dataSize,0, \
            (struct sockaddr *)&addr,addrSize);

        buffer[0] = '\0';

        recvfrom(endpointfd,buffer,64,0,NULL, NULL);

        counter = 0;
        c = '0';
        while(c != '\n' || '\0')
        {
            c = buffer[counter];
            counter++;
        }
        if (c == '\n')
        {
            buffer[counter+1] = '\0';
        }  

        if(channel)
        {
            /* Pressure sensor sensitivity = 6.25 cm/V  */
            tank1 = (float) atof(buffer)*6.25;
            channel = 0;
        }
        else
        {
            /* Pressure sensor sensitivity = 6.25 cm/V */
            tank0 = (float) atof(buffer)*6.25;
            channel = 1;
        }

#ifdef __unix__
        /* 25 data about tanks per sec */
        usleep(20000);
		if(initOK) glutPostRedisplay();
#endif
    }
 
}

/* Opengl Globals */
static GLfloat light0_position[] = { 0.0, 0.00, 1.5, 1.0 }; 
static GLfloat light1_position[] = { -1.0, 0.50, -1.50, 1.0 }; 
static int move_x = 90;
static int move_y = 0;

void init()
{
    glClearColor (1.0,1.0,1.0,0.0);
  
    GLfloat shine[] = {100.0};
    glMaterialfv(GL_FRONT, GL_SHININESS, shine);
  
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glShadeModel (GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
	initOK = 1;
}

void display(void)
{
    if(getR)
    {
        exit(getR);
    }

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable (GL_LIGHTING);  

    /* Light */
    glPushMatrix ();
    glLightfv (GL_LIGHT0, GL_POSITION, light0_position); 
    glLightfv (GL_LIGHT1, GL_POSITION, light1_position); 
    glPopMatrix ();

    /* View */
    glPushMatrix ();
    glTranslatef (0.0, 0.0, -5.0);
    glRotated (move_x,0.0,1.0,0.0);
    glRotated (move_y,1.0,0.0,0.0);   

    /* Material */ 
    GLfloat mat_diffuse[] = { 0.1, 0.9, 0.9, 0.2 };
    GLfloat mat_diffuse2[] = { 0.1, 0.9, 0.9, 1.0 };
    GLfloat mat_shininess[] = { 5.0 };    
    
    /* Tank 1  */
    glPushMatrix ();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
    glTranslatef (0.0, 1.0, 0.0);
    glRotated (90,1.0,0.0,0.0);
    
    GLUquadric * quad = gluNewQuadric();
    gluCylinder (quad,0.3, 0.3, 1.0, 1000, 1000);   

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
  
    GLUquadric * quadD = gluNewQuadric();
    GLUquadric * quadD2 = gluNewQuadric();
    gluDisk (quadD,0.03,0.3,50,50);
    
    glTranslatef (0.0, 0.0, 1.0);
    gluDisk (quadD2,0.03,0.3,50,50); 

    glPopMatrix();
     
    /* Tank 2  */
    glPushMatrix ();
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
    glTranslatef (0.0, -0.5, 0.0);
    glRotated (90,1.0,0.0,0.0);
    
    GLUquadric * quad2 = gluNewQuadric();
    gluCylinder (quad2,0.3, 0.3, 1.0, 1000, 1000);       

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
 
    GLUquadric * quadD3 = gluNewQuadric();
    GLUquadric * quadD4 = gluNewQuadric();
    gluDisk (quadD3,0.03,0.3,50,50);
    
    glTranslatef (0.0, 0.0, 1.0);
    gluDisk (quadD4,0.0,0.3,50,50); 

    glPopMatrix();

    /* Pipe */
    glPushMatrix ();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
    glTranslatef (0.0, 0.0, 0.0);
    glRotated (90,1.0,0.0,0.0);
    
    GLUquadric * quad3 = gluNewQuadric();
    gluCylinder (quad3,0.03, 0.03, 1.0, 50, 50);       
    
    glPopMatrix();

    /* Pump */
    glPushMatrix ();

    glTranslatef (0.275, -1.51, 0.325);
    glRotated (90,0.0,0.0,1.0);    
    glutSolidCube(0.15); 

    glPopMatrix();

    /* Water Level 1 */
    glPushMatrix ();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
    glTranslatef (0.0, 0.0, 0.0);
    glRotated (-90,1.0,0.0,0.0);
    
    GLUquadric * quadW = gluNewQuadric();
    gluCylinder (quadW,0.31, 0.31,\
        (GLdouble) 1.0*(tank0/30.0), 1000, 1000);   

    glPopMatrix ();

    /* Water Level 2 */
    glPushMatrix ();

    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
 
    glTranslatef (0.0, -1.5, 0.0);
    glRotated (-90,1.0,0.0,0.0);
    
    GLUquadric * quadW2 = gluNewQuadric();
    gluCylinder (quadW2,0.31, 0.31,\
        (GLdouble) 1.0*(tank1/30.0), 1000, 1000);   

    glPopMatrix ();


    /* Flush */
    
    glPopMatrix ();
    glPopMatrix ();
    glPopMatrix ();
    glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(40.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
            {
                printf("Mouse Down, left.\n");
            }
            break;
        case GLUT_MIDDLE_BUTTON:
            if (state == GLUT_DOWN)
            {
                printf("Mouse Down, middle.\n");
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_DOWN)
            {
                printf("Mouse Down, right.\n"); 
            }
            break;
      default:
         break;
   }
}

void keyboard (unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:   /* ESC */
            exit(0);
            break;
        case 82: /* Go to next case */
        case 114: /* Reset Positions */
            move_x = 90;
            move_y = 0;
            glutPostRedisplay();
            break;
        default:
            break;
   }
}

void keyboardSpecials (int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_RIGHT:
            move_x = (move_x + 15) % 360;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:
            move_x = (move_x + 345) % 360;
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            move_y = (move_y + 15) % 360;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            move_y = (move_y + 345) % 360;
            glutPostRedisplay();
            break;       
        default:
            break;
   }
}

int main(int argc, char ** argv)
{
    pthread_t thread;
    if(pthread_create(&thread,NULL,getData,NULL))
    {
#ifdef DEBUG_MODE
        printf("Error creating \'getData\' thread.\n");
#endif
        return -1;
    }
	getR = 0;
	initOK = 0;	

    /* Graphics Here! */
    glutInit(&argc, argv);        
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Simutank GUI");
    init();
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecials);
    glutMainLoop();
    
    pthread_join(thread,NULL);

    return 0;
}

