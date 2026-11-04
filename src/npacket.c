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

int main(int argc, const char* argv[])
{
    //check program parameters
    if(argc>1)
    {
        printf("parameter have to be processed: %i\n", argc);    
        for(size_t i; i < argc; i++)
        {
            printf("%i: %s\n", i, argv[i]);

        }

    }





    #ifdef NCURSES 
        initscr();
        
        cbreak(); 
        scrollok(stdscr, true);
        timeout(10);
        //printw("toll %s", "stefan");

    #endif
    //mvprintw(0,10,"hallo");
    //mvprintw(2,10,"stefan");

    return 0;
    
    int sock_raw;
    uint8_t buffer[65535];

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

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
    
    int ch;

    while(running)
    {
        ch = getch();		
        
        if(ch == 'q')
        {
            //break;
            running = false;
        }
        
        frame.size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);

        //save timestamp
        clock_gettime(CLOCK_REALTIME, &timeStampFrame);
        
        frame.timestamp.tv_sec = timeStampFrame.tv_sec - startCapturing.tv_sec;
        frame.timestamp.tv_nsec = timeStampFrame.tv_nsec - startCapturing.tv_nsec;

        if(frame.size < 0)
        {
            perror("Error when Reading Data...\n");
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

                #ifdef NCURSES
                
                   printw("%s",ethernetString); 
                #else

                    printf("%s",ethernetString); 
                #endif 
                

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

                #ifdef NCURSES
                    printw(" || %s",IPv4String); 
                #else
                    printf(" || %s",IPv4String); 
                #endif 
                
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

            #ifdef NCURSES

                //refresh();          // writes the windows
                //getch();            //waits for input

            #endif

        }
        #ifdef NCURSES
            printw("\n"); 
            refresh();          // writes the windows
        #else 
            printf("\n");
        #endif 
        

    //getch();            //waits for input
    
    }   
    


    #ifdef NCURSES

    endwin();           //ends window
    //nocbreak();

    #endif



    return 0;
}

