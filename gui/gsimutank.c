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
#endif
    }
 
}

/* Opengl Globals */
GLfloat light_position[] = { 2.0, 3.0, -1.5, 0.0 }; 

void init()
{
    glClearColor (1.0,1.0,1.0,0.0);
    
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 30.0 }; 
    GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glShadeModel (GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix ();

    glTranslatef (0.0, 0.0, -5.0);
    glRotated (90,1,0,0);    

    glLightfv (GL_LIGHT0, GL_POSITION, light_position);
    
    /* Tank 1  */
    GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_diffuse[] = { 0.1, 0.9, 0.9, 0.5 };
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_shininess[] = { 0.0 };
    GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);    
 
    GLUquadric * quad = gluNewQuadric();
    gluCylinder (quad,0.4, 0.4, 0.5, 100, 100); 
   
    glEnable (GL_LIGHTING);  
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
   switch (key) {
      case 27:   /* ESC */
         printf("ESC Pressed.\n");
         break;
      default:
         break;
   }
}

int main(int argc, char ** argv)
{
/*    pthread_t thread;
    if(pthread_create(&thread,NULL,getData,NULL))
    {
#ifdef DEBUG_MODE
        printf("Error creating \'getData\' thread.\n");
#endif
        return -1;
    }
*/
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
    glutMainLoop();
    
    ////pthread_join(thread,NULL);
    return 0;
}

