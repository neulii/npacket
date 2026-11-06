#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <ncurses.h>

#include <time.h>

#include "parsingPrinting.h"

//#define NCURSES 

enum gMode
{
    MODE_NCURSES,
    MODE_CLI
};

int main(int argc, const char* argv[])
{

    enum gMode mode;

    //check program parameters
    if(argc>1)
    {
        //printf("parameters: %i\n",argc);
        
        //printf("argument 1 %s\n", argv[1]); 
        if(strcmp(argv[1],"-cli") == 0)
        {
            mode = MODE_CLI;
            printf("mode cli is set!\n");
        }
        else
        {
            printf("parameter not found!\n");
            return 1;
        }
    }
    else
    {
        mode = MODE_NCURSES;
        printf("mode ncurses is set!\n");
    }
    
    //TODO make ncurses global
    initscr();
    noecho();
    curs_set(0);
    //scrollok(stdscr, true);

    if(has_colors() == FALSE)
    {
        endwin();
        printf("your Terminal does not support colors!!");
        return 1;
    }

    init_pair(1, COLOR_RED, COLOR_BLACK);

    int windowX = 5;
    int windowY = 2;
    int width = 250;
    int height = 70;

    WINDOW *windowFrame = newwin(height, width, windowY, windowX);
    //box(windowFrame, 0,0);
    //wrefresh(windowFrame);

    WINDOW *windowText = derwin(windowFrame, height - 2, width - 2, 1,1);
    scrollok(windowText, TRUE);

    int sock_raw;
    uint8_t buffer[65535];

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    //timeout(1);

    //exit when error by opening the socket
    if(sock_raw < 0)
    {
        perror("Socket Error!\n");
        return 1;
    }
   
    EthernetFrame frame;
    IPV4 ipv4;
    
    bool running = true;
    
    //starting Time
    struct timespec startCapturing;
    struct timespec timeStampFrame;

    //get Time when capturing starts
    clock_gettime(CLOCK_REALTIME, &startCapturing);

    //counter
    long frameCounter = 0;
    
    //int ch;

    while(running)
    {
        /*
        timeout(1);
        ch = getch();		
        if(ch == 'q')
        {
            //break;
            running = false;
            endwin();
        }
        */ 
        frame.size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);

        //save timestamp
        clock_gettime(CLOCK_REALTIME, &timeStampFrame);
        
        frame.timestamp.tv_sec = timeStampFrame.tv_sec - startCapturing.tv_sec;
        frame.timestamp.tv_nsec = timeStampFrame.tv_nsec - startCapturing.tv_nsec;

        if(frame.size < 0)
        {
            perror("Error when Reading Data...");
            return 1;
        }
        
        //parsing ethernet data from buffer to Frame
        parseBufferToFrame(&frame, buffer);

        frame.frameNr = frameCounter;
        frameCounter++;
        
        //build etherent output line
        char *ethernetString = NULL;
        ethernetString = buildEthernetOutput(&frame);
        
        if(ethernetString==NULL)
        {
            printf("error by return outputtext");
            return 1;
        }

        wprintw(windowText, "%s",ethernetString); 
        
        free(ethernetString);

        //converts 2 bytes in one datatype for comparing in switch
        int16_t etherType = ((uint16_t)frame.type[0] << 8) | frame.type[1];

        switch (etherType)
        {
            //if IP Protocol 0x0800
            case 0x0800:
                //create frame for access (because its from a void pointer)
                frame.nextLayer = &ipv4;
                IPV4 *packet= frame.nextLayer;

                //set flag and getting memory 
                char flag[] = "ipv4";
                //char *flagTemp = malloc(sizeof flag);
                packet->identifier = malloc(sizeof flag);
                
                if(packet->identifier == NULL)
                {
                    return 1;
                }

                strcpy(packet->identifier,flag);

                //parsing data to packet type
                parseBufferToPacket(packet, buffer);

                //print ipv4 specific part
                char *IPv4String = NULL;
                IPv4String = buildIPv4Output(&frame);
                
                if(IPv4String==NULL)
                {
                    printf("error by return outputtext");
                    return 1;
                }

                wprintw(windowText, " || %s",IPv4String); 
                
                //cleanup
                free(IPv4String);
                free(packet->identifier);
                
                break;
            
            //if ARP Protocol 0x0806
            case 0x0806:
                //create frame for access (because its from a void pointer)
                //just code ... isnt implemented 
                //ARP *ARPpacket= frame.nextLayer;
                
                //IPV4 Protocol - Type
                //memcpy(&packet->protocol[0], &buffer[posIPV4 + IP_PROT_OFFSET], sizeof (packet->protocol[0]) * IP_PROT_LENGTH);
                
                // -> here print the specific part
                
                //printOutputLine(&frame);
                break;
            default:
                break;
        }
        
          
        wprintw(windowText, "\n"); 
        
        box(windowFrame, 0,0);
        wrefresh(windowFrame);
        wrefresh(windowText);
        
        refresh();          // writes the windows
    
    }   
    
    endwin();           //ends window
    
    return 0;
}

