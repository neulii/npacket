#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <ncurses.h>


/*
    EthernetFrame -> IPV4 -> TCP
*/

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

    if(sock_raw < 0)
    {
        perror("Socket Error!\n");
        return 1;

    }

    printf("Beginning reading Packets...\n");
    

    int height = 40;
    int width = 100;
    int start_y = 10;
    int start_x = 10;
   
    WINDOW *title = newwin(3, 100, 2, 5);
    WINDOW *windowFrame = newwin(45, 100, 4, 5); //rahmenfenster
    WINDOW *win = derwin(windowFrame,44,98, 1,2); //window for text -> scrolling

    scrollok(win, TRUE); //allow srolling just the windo

    //define Ehternet Frame struct
    struct
    {   
        uint8_t dst[6];
        uint8_t src[6];
        uint8_t type[2];
    }
    typedef EthernetFrame;

    bool running = true;
    
    
    while(running)
    {
        EthernetFrame frame;

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

        if (ch == 'q') {
            //running = false; 
            break;
        }

        ssize_t data_size = recvfrom(sock_raw,buffer,sizeof(buffer), 0, NULL, NULL);
        if(data_size < 0)
        {
            perror("Error when Reading Data...\n");
            return 1;
        }
        //mvprintw(10,1,"hallo");
        //printf("%i\n", data_size);
        
        //printw("%li\n", data_size);
        //printw("%i\n",buffer[4]);
        wprintw(win, "%6li  ||  ", data_size);

        //Destination Mac
        for(size_t i = 0; i < 6; i++)
        {
            memcpy(&frame.dst[i], &buffer[i],sizeof (frame.dst[0]));

            //wprintw(win, "%2x ", buffer[i]);
            wprintw(win, "%2x ", frame.dst[i]);
        }
        wprintw(win, " || ");
        
        //Source Mac
        for(size_t i = 6; i < 12; i++)
        {
            //wprintw(win, "%2x ", buffer[i]);
            memcpy(&frame.src[i], &buffer[i],sizeof (frame.src[0]));
            wprintw(win, "%2x ", frame.src[i]);

        }
        
        //Ethertype
        wprintw(win, " || ");
        for(size_t i = 12; i < 14; i++)
        {
            //wprintw(win, "%2x", buffer[i]);
            memcpy(&frame.type[i], &buffer[i],sizeof (frame.type[0]));
            wprintw(win, "%2x", frame.type[i]);
        }
        wprintw(win, " || ");

        
        //IPV4 Protocol - Type
        for(size_t i = 23; i < 24; i++)
        {
            wprintw(win, "%4i", buffer[i]);

        }
        wprintw(win, "\n");
        char text[100] = "super";
        strcat(text, "asdf");
        //text = "geil";

        //wprintw(win, "%s\n", text);
        refresh();
    }
    wprintw(win, "super");
    endwin();



    return 0;
}
