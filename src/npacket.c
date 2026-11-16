
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>
#include <pcap.h>
#include <time.h>

#include "parsingPrinting.h"

char selInterf[50];

pcap_if_t *alldevs = NULL;                 //all interfaces 
pcap_if_t *dev = NULL;                     //interface for working with
pcap_t *handle = NULL;

char errbuf[PCAP_ERRBUF_SIZE];      //errorbuffer for pcap

size_t frameArraySize = 100;
EthernetFrame *capturedFrames = NULL;   //all Frames that are captured
EthernetFrame *framesToDisplay = NULL;




void cleanUpAndExit();


int main(int argc, const char* argv[])
{
    
    //when interface detection fails
    if (pcap_findalldevs(&alldevs, errbuf) != 0) {
        printf("%s\n", errbuf);
        return 1;
    }   

    //check program parameters
    if(argc>1)
    {
        //printf("%s\n", argv[1]);
        strcpy(selInterf, argv[1]);
        printf("%s\n", selInterf);
        
    }
    else            //when no cli parameters list all interfaces
    
    {
        printf("usage: npacket [interfacename]\n");
        for (dev = alldevs; dev != NULL; dev = dev->next) 
        {
            printf("%s  //  ", dev->name);

            if (dev->description != NULL) 
            {
                printf("%s\n", dev->description);
            } 
            else 
            {
                printf("\n");
            }
        }
        return 0;
    }
    
    
    initscr();
    noecho();
    curs_set(0);

    if(has_colors() == FALSE)
    {
        printf("your Terminal does not support colors!!");
        endwin();
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
    WINDOW *windowText = derwin(windowFrame, height - 2, width - 2, 1,1);

    scrollok(windowText, TRUE);

    keypad(stdscr, TRUE);           //for special keys

    const unsigned char *buffer;    //buffer for getting data from NIC

    //create pcap handle
    handle = pcap_open_live(    selInterf, // interfaceName
        65535, // bufferSize
        1,     // promiscouse Mode=1
        1,  // timeout
        errbuf
    );
  
    if (handle == NULL)
    {
        printf("Fehler: %s\n", errbuf);
        endwin();
        return 1;
    }

    struct pcap_pkthdr *header;

    EthernetFrame frame;
    IPV4 ipv4;
   
    
    capturedFrames = malloc(sizeof(EthernetFrame) * frameArraySize);

 
    bool running = true;
    
    //starting Time
    struct timespec startCapturing;     //time of Starting the capturing Loop
    struct timespec timeStampFrame;     //time of the Capturering
    struct timespec deltaLoop;          //delta from loop to loop

    //get Time when capturing starts
    clock_gettime(CLOCK_REALTIME, &startCapturing);

    //counter
    long frameCounter = 0;
    
    // int ch;

    while(running)
    {
        // timeout(1);
        // ch = getch();		
        // if(ch == 'q')
        // {
        //     //break;
        //     running = false;
        //     endwin();
        // }
        // if(ch == KEY_UP)
        // {
        //     init_pair(1, COLOR_WHITE, COLOR_BLUE);
        // }
        // if(ch == KEY_DOWN)
        // {
        //    init_pair(1, COLOR_WHITE, COLOR_YELLOW);
        // }
        
        //check array for saving frames if size is large enough
        size_t arrayFrameBuffer = 10;
        size_t sizeNewMemory = 100;

        if(frameCounter + arrayFrameBuffer >= frameArraySize)
        {
            EthernetFrame *temp = realloc(capturedFrames, sizeof(capturedFrames[0]) * (frameArraySize + sizeNewMemory));

            if(temp==NULL)
            { 
                printf("Error when allocating new Memory for frames...");
                free(capturedFrames);
                cleanUpAndExit();
            }
            else
            {
                capturedFrames = temp;
                frameArraySize = frameArraySize + sizeNewMemory;
            }
            
        }

        int result = pcap_next_ex(handle, &header, &buffer);

        if(result!=1)
        {
            perror("Error when Reading Data...");
            cleanUpAndExit();
        }
        //frame.size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);
        frame.size = header->len;
        
        //save timestamp
        clock_gettime(CLOCK_REALTIME, &timeStampFrame);
        
        frame.timestamp.tv_sec = timeStampFrame.tv_sec - startCapturing.tv_sec;
        frame.timestamp.tv_nsec = timeStampFrame.tv_nsec - startCapturing.tv_nsec;

        
        //parsing ethernet data from buffer to Frame
        //TODO
        parseBufferToFrame(&frame, buffer);

        frame.frameNr = frameCounter;
        
        //build etherent output line
        char *ethernetString = NULL;
        ethernetString = buildEthernetOutput(&frame);
        
        if(ethernetString==NULL)
        {
            printf("error by return outputtext");
            cleanUpAndExit();
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
                IPV4 *packetArray = malloc(sizeof(IPV4));
                capturedFrames->nextLayer = packetArray;



                //set flag and getting memory 
                char flag[] = "ipv4";
                //char *flagTemp = malloc(sizeof flag);
                packet->identifier = malloc(sizeof flag);
                
                if(packet->identifier == NULL)
                {
                    cleanUpAndExit();
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
                    cleanUpAndExit();
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
        
        capturedFrames[frameCounter] = frame;
        frameCounter++;
       
        //memcpy(&capturedFrames[frameCounter],&frame,sizeof(frame));
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
    
    pcap_close(handle);
    pcap_freealldevs(alldevs);
    pcap_freealldevs(dev);
    
    return 0;
}

void cleanUpAndExit()
{
    pcap_close(handle);
    pcap_freealldevs(alldevs);
    pcap_freealldevs(dev);
    endwin();
    exit(1);
    

}