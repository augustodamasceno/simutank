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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __unix__
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#define DEBUG_MODE


/* Put this code in a thread and share info with opengl graphics */
int main(int argc, char ** argv)
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
        return -1;
    }

#ifdef DEBUG_MODE
    printf("Socket opened.\n");
#endif

    if (connect(endpointfd, (struct sockaddr *) &addr, addrSize) < 0)
    {
#ifdef DEBUG_MODE
        printf("ERROR connecting socket.\n");
#endif
        return -2;
    }

#ifdef DEBUG_MODE
    printf("Socket connected.\n");
#endif

    char c = '0';
    int counter;
    while(1)
    {
        dataSize = sprintf(buffer,"READ 0\n");

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

        printf("Received from server: %s\n",buffer);

#ifdef __unix__
        usleep(40000);
#endif
    }
    
    return 0;
}
