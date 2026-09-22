#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <ncurses.h>




int main(void)
{
    //init ncurse
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    int sock_raw;
    uint8_t buffer[65535];

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if(sock_raw < 0)
    {
        perror("Socket Error!\n");
        return 1;

    }

    printf("Beginning reading Packets...\n");
    

    bool running = true;
    
    while(running)
    {
        int ch = getch();

        if (ch == 'q') {
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
        printw("size: %6li  ||  ", data_size);


        for(size_t i = 0; i < 6; i++)
        {
            printw("%2x ", buffer[i]);
        }
        printw(" || ");

        for(size_t i = 6; i < 12; i++)
        {
            printw("%2x ", buffer[i]);
        }

        printw(" || ");
        for(size_t i = 12; i < 14; i++)
        {
            printw("%2x", buffer[i]);

        }
        printw("\n");

        refresh();
    }
    endwin();



    return 0;
}
