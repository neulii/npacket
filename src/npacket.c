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

#include "types.h"

void printOutputLine(EthernetFrame* frame, WINDOW* window);


int main(void)
{
    //init ncurse
    initscr();

    //check if colors are possible in terminal
    if(has_colors() == FALSE)
    {
        endwin();
        printf("Terminal cant show Colors!!!");
        exit(1);
    }
    start_color();
    

    //define color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    //scrollok(stdscr, TRUE);  //scrolling the whole terminal 

    int sock_raw;
    uint8_t buffer[65535];

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    //exit when error by opening the socket
    if(sock_raw < 0)
    {
        perror("Socket Error!\n");
        return 1;

    }
    
    //paint Window
    
    int width = 100;
    int titleHeight = 3;
    int windowFrameHeight = 45; 
    int titleStartX = 5;
    int titleStartY = 2;
    int windowStartX = 5;
    int windowStartY = 4;

    //windows for title
    WINDOW *title = newwin(
        titleHeight, 
        width, 
        titleStartY, 
        titleStartX
    );
   
    //rahmen -> aussenfenster
    WINDOW *windowFrame = newwin(
        windowFrameHeight, 
        width, 
        windowStartY, 
        windowStartX 
    );
    
    //create subwindow  
    //window for text -> scrolling

    WINDOW *win = derwin(
        windowFrame,
        44,
        98,
        1,
        2
    );
    
    scrollok(win, TRUE); //allow srolling just the windo

    
    EthernetFrame frame;
    IPV4 ipv4;
    frame.nextLayer = &ipv4;

    //textline for output
    
    bool running = true;
    
    while(running)
    {

        
        box(title, 0,0);   
        wrefresh(title);
        
        box(windowFrame,0,0); 
        wrefresh(windowFrame);
        
        //box(win,1,1);
        wrefresh(win);
        wrefresh(windowFrame);
        
        wattron(title, COLOR_PAIR(1));;
        mvwprintw(title,1,1,"  size   ||        src          ||        dst         || type ||  proto");
        
        wattroff(title, COLOR_PAIR(1));;
    
               
        int ch = getch();

        //quit loop by pressing q
        if (ch == 'q') {
            break;
        }

        frame.size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);
        if(frame.size < 0)
        {
            perror("Error when Reading Data...\n");
            return 1;
        }
        
        

        //Destination Mac
        memcpy(&frame.dst[0], &buffer[ETH_DST_OFFSET],sizeof(frame.dst[0]) * ETH_DST_LENGTH);

        //Source Mac
        memcpy(&frame.src[0], &buffer[ETH_SRC_OFFSET],sizeof (frame.src[0]) * ETH_SRC_LENGTH);
        
        //Ethertype
        memcpy(&frame.type[0], &buffer[ETH_TYPE_OFFSET],sizeof (frame.type[0])*ETH_TYPE_LENGTH);
        
        uint8_t posIPV4 = ETH_PAYLOAD_OFFSET;

        //if IP Protocol 0x0800
        if(frame.type[0]==8 && frame.type[1] == 0)
        {
            
            //create frame for access (because its from a void pointer)
            IPV4 *packet= frame.nextLayer;
            
            //IPV4 Protocol - Type
            memcpy(&packet->protocol[0], &buffer[posIPV4 + IP_PROT_OFFSET], sizeof (packet->protocol[0]) * IP_PROT_LENGTH);
            
        }
        printOutputLine(&frame, win);

        refresh();
    }
    wprintw(win, "super");
    endwin();

    return 0;
}

//prints the out a line in the window with ethernet
void printOutputLine(EthernetFrame* frame, WINDOW* window)
{
 
    char outputText[100] = "";
    char outputTemp[100] = "";

    //size
    sprintf(outputTemp, "%5li  ||  ", frame->size);
    strcat(outputText, outputTemp);

    //dst mac
    for(size_t i = 0; i < ETH_DST_LENGTH; i++)
    {
        sprintf(outputTemp, "%2x ", frame->dst[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //src mac
    for(size_t i = 0; i < ETH_SRC_LENGTH; i++)
    {
        sprintf(outputTemp, "%2x ", frame->src[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //ethertype
    for(size_t i = 0; i < ETH_TYPE_LENGTH; i++)
    {
        sprintf(outputTemp, "%2x ", frame->type[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");

    
    
    //ip 
    IPV4 *packet= frame->nextLayer;
    
    for(size_t i = 0; i < IP_PROT_LENGTH; i++)
    {
        sprintf(outputTemp, "%4i", packet->protocol[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, "\n");
  
    //prints the finished line
    wprintw(window,outputText);
}
