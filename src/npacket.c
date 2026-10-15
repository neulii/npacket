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
    init_pair(3,COLOR_BLUE, COLOR_BLACK);

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
    IPV4 *packet= frame.nextLayer;

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

        ssize_t data_size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0)
        {
            perror("Error when Reading Data...\n");
            return 1;
        }
        
        wprintw(win, "%6li  ||  ", data_size);

        //Destination Mac
        memcpy(&frame.dst[0], &buffer[ETH_DST_OFFSET],sizeof(frame.dst[0]) * ETH_DST_LENGTH);

        //Source Mac
        memcpy(&frame.src[0], &buffer[ETH_SRC_OFFSET],sizeof (frame.src[0]) * ETH_SRC_LENGTH);
        
        //Ethertype
        memcpy(&frame.type[0], &buffer[ETH_TYPE_OFFSET],sizeof (frame.type[0])*ETH_TYPE_LENGTH);
        
        printOutputLine(&frame, win);
        uint8_t posIPV4 = ETH_PAYLOAD_OFFSET;

        //IPV4 Protocol - Type
        memcpy(
            
            &packet->protocol[0], 
            &buffer[posIPV4 + IP_PROT_OFFSET],
            sizeof (packet->protocol[0]) * IP_PROT_LENGTH
        );
        
        for(size_t i = 0; i < IP_PROT_LENGTH; i++)
        {
            wprintw(win, "%4i", packet->protocol[i]);
        }





        wprintw(win, "\n");
        //char text[100] = "super";
        //strcat(text, "asdf");
        //text = "geil";

        //wprintw(win, "%s\n", text);
        refresh();
    }
    wprintw(win, "super");
    endwin();



    return 0;
}

void printOutputLine(EthernetFrame* frame, WINDOW* window)
{
 
     char outputText[100] = "";
     char outputTemp[100] = "";

     for(size_t i = 0; i < ETH_DST_LENGTH; i++)
        {
            sprintf(outputTemp, "%2x ", frame->dst[i]);
            strcat(outputText, outputTemp);
            
            //wprintw(win, "%2x ", frame.dst[i]);
        }
     wprintw(window, outputText,"");
     wprintw(window, " || ");
        
    for(size_t i = 0; i < ETH_SRC_LENGTH; i++)
    {
        wprintw(window, "%2x ", frame->src[i]); 
    }
    wprintw(window, " || ");

    for(size_t i = 0; i < ETH_TYPE_LENGTH; i++)
    {
         wprintw(window, "%2x", frame->type[i]);
    }
    wprintw(window, " || ");







}


