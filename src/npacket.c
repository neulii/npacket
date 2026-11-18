
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

void printEthernetFramesToWindow(WINDOW *win, EthernetFrame* frames, size_t start, size_t end);
void copyVisibleFramesToArray(EthernetFrame *captured, EthernetFrame *framesToDisplay);

void cleanUpAndExit(void);


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
        //TEMP FOR DEVELOPING
        strcpy(selInterf, alldevs[0].name);
        //return 0;
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

    //TODO Colorinitialising dont works now
    #define ORANGE 10

    init_color(ORANGE, 1000, 1000, 1000);


    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_RED);
    init_pair(4, COLOR_WHITE, COLOR_YELLOW);
    init_pair(5, COLOR_WHITE, COLOR_GREEN);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_CYAN);
    init_pair(8, COLOR_WHITE, ORANGE);

    
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

    //scrollok(windowText, TRUE);

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

    //EthernetFrame frame;
    //IPV4 ipv4;
   
    
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
    
    int ch;

    while(running)
    {
        EthernetFrame frame = {0};
        timeout(1);
        ch = getch();		
        if(ch == 'q')
        {
            //break;
            running = false;
            //endwin();
        }
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

        //wprintw(windowText, "%s",ethernetString); 
        
        free(ethernetString);

        //converts 2 bytes in one datatype for comparing in switch
        int16_t etherType = ((uint16_t)frame.type[0] << 8) | frame.type[1];

        switch (etherType)
        {
            //if IP Protocol 0x0800
            case TYPE_IPV4:
                IPV4 *packet = malloc(sizeof *packet);
                if (packet == NULL) {
                    cleanUpAndExit();
                }

                frame.nextLayer = packet;
                parseBufferToPacket(packet, buffer);

                break;
            
            //if ARP Protocol 0x0806
            case TYPE_ARP:
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
       
        
        //wprintw(windowText, "\n"); 

        //int x = getcurx(windowText);
        //int y = getcury(windowText);

        //mvwchgat(windowText, cursorLine, 0, width, A_NORMAL, 2, NULL);
        //mvwchgat(windowText, cursorLine, 0, width, A_NORMAL, 1, NULL);
        
        //wmove(windowText,y,0);
       
        

        //print the last frames
        unsigned int showingItems = 50;

        unsigned int startPrinting = 0;

        if (frameCounter < showingItems)
        {
            startPrinting = 0;
        }
        else
        {
            startPrinting = frameCounter-showingItems;
        }

        printEthernetFramesToWindow(windowText, capturedFrames,startPrinting, frameCounter);

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


void printEthernetFramesToWindow(WINDOW *win, EthernetFrame* frames, size_t start, size_t end)
{
    wmove(win,0,0); //beginning left top

    for(size_t i = start; i < end; i++)
    {
        int16_t etherType = ((uint16_t)frames[i].type[0] << 8) | frames[i].type[1];
        
        char *outputEthernet = buildEthernetOutput(&frames[i]);
        
        switch (etherType)
        {
            case 0x0800:
            
                IPV4 *packet = frames[i].nextLayer;
                
                switch (*packet->protocol)
                {
                    case IPV4_TCP:
                        char *outputIPv4 = buildIPv4Output(&frames[i]);
                        //init_pair(1, COLOR_WHITE, COLOR_BLUE);
                        wattron(win,COLOR_PAIR(2));
                        wprintw(win, "%s || TCP || %s\n",outputEthernet, outputIPv4); 
                        //wrefresh(win);
                        wattroff(win,COLOR_PAIR(1));
                        break;
                    case IPV4_UDP:
                        wattron(win,COLOR_PAIR(3));
                        wprintw(win, "%s || UDP\n", outputEthernet); 
                        wattroff(win,COLOR_PAIR(1));
                        break;
                    
                    case IPV4_ICMP:
                        wattron(win,COLOR_PAIR(8));
                        wprintw(win, "%s || ICMP\n", outputEthernet); 
                        wattroff(win,COLOR_PAIR(1));
                        break;
                    
                    default:
                        wattron(win,COLOR_PAIR(1));
                        wprintw(win, "%s || IPV4 == not parsed yet ==\n", outputEthernet); 
                        wattroff(win,COLOR_PAIR(1));
                    
                        break;
                }
                   
                break;
            
            case TYPE_ARP:
                wattron(win,COLOR_PAIR(4));
                wprintw(win, "%s || ARP\n", outputEthernet); 
                wattroff(win,COLOR_PAIR(1));
                
                break;
                
            
            default:
                wattron(win,COLOR_PAIR(5));
                wprintw(win, "%s || ALL THE REST\n", outputEthernet); 
                wattroff(win,COLOR_PAIR(1));

               

            break;
        }
    }

    return;
}

void copyVisibleFramesToArray(EthernetFrame *captured, EthernetFrame *framesToDisplay)
{


}
