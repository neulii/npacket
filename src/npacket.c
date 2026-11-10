
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>
#include <pcap.h>
#include <time.h>

#include "parsingPrinting.h"

enum gMode
{
    MODE_NCURSES,
    MODE_CLI
};

char selInterf[50];

int main(int argc, const char* argv[])
{
    pcap_if_t* alldevs;
    pcap_if_t *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    //int userInput = 0;
    
    //when interface detection fails
    if (pcap_findalldevs(&alldevs, errbuf) != 0) {
        printf("%s\n", errbuf);
        return 1;

    }   
    int i = 0;

    //check program parameters
    if(argc>1)
    {
       //printf("%s\n", argv[1]);
       strcpy(selInterf, argv[1]);
       printf("%s\n", selInterf);

    }
    else

    {
        printf("usage: npacket [interfacename]\n");
        for (dev = alldevs; dev != NULL; dev = dev->next) 
        {
            printf("[%d] %s  //  ", i, dev->name);

            if (dev->description != NULL) 
            {
                printf("%s\n", dev->description);
            } 
            else 
            {
                printf("\n");
            }
            i++;
        }
        return 0;
    }
    
    //TODO make ncurses global
    initscr();
    noecho();
    curs_set(0);

    if(has_colors() == FALSE)
    {
        endwin();
        printf("your Terminal does not support colors!!");
        return 1;
    }
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    int terminalX = COLS;
    int terminalY = LINES;

    int windowX = 2;
    int windowY = 2;
    int width = terminalX-4;
    int height = terminalY-5;
    //int height = 20; //test height

    int cursorLine = 0;

    WINDOW *windowFrame = newwin(height, width, windowY, windowX);
    //box(windowFrame, 0,0);
    //wrefresh(windowFrame);

    WINDOW *windowText = derwin(windowFrame, height - 2, width - 2, 1,1);
    scrollok(windowText, TRUE);

    keypad(stdscr, TRUE);       //for special keys

    //int sock_raw;
    //uint8_t buffer[65535];
    const unsigned char *buffer;

    //sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    //exit when error by opening the socket
    // if(sock_raw < 0)
    // {
    //     perror("Socket Error!\n");
    //     return 1;
    // }
    pcap_t *handle = pcap_open_live(    selInterf, // interfaceName
        65535, // bufferSize
        1,     // promiscouse Mode=1
        1,  // timeout
        errbuf
    );
  
    
    if (handle == NULL)
    {
        printf("Fehler: %s\n", errbuf);
        return 1;
    }

    struct pcap_pkthdr *header;

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
        // timeout(1);
        // ch = getch();		
        if(ch == 'q')
        {
            //break;
            running = false;
            endwin();
        }
        if(ch == KEY_UP)
        {
            init_pair(1, COLOR_WHITE, COLOR_BLUE);
        }
        if(ch == KEY_DOWN)
        {
           init_pair(1, COLOR_WHITE, COLOR_YELLOW);
        }
        
        int result = pcap_next_ex(handle, &header, &buffer);

        //frame.size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);

        //save timestamp
        clock_gettime(CLOCK_REALTIME, &timeStampFrame);
        
        frame.timestamp.tv_sec = timeStampFrame.tv_sec - startCapturing.tv_sec;
        frame.timestamp.tv_nsec = timeStampFrame.tv_nsec - startCapturing.tv_nsec;

        if(result!=1)
        {
            perror("Error when Reading Data...");
            return 1;
        }
        
        //parsing ethernet data from buffer to Frame
        //TODO
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

        //int x = getcurx(windowText);
        int y = getcury(windowText);

        mvwchgat(windowText, cursorLine, 0, width, A_NORMAL, 2, NULL);
        mvwchgat(windowText, cursorLine, 0, width, A_NORMAL, 1, NULL);
        
        wmove(windowText,y,0);
       
        box(windowFrame, 0,0);
        
        wrefresh(windowFrame);
        wrefresh(windowText);
        
        refresh();          // writes the windows
    }   
    
    endwin();           //ends window
    
    return 0;
}