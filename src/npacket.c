#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
//#include <ncurses.h>

#include "types.h"

char* buildEthernetOutput(EthernetFrame* frame);
char* buildIPV4Output(EthernetFrame* frame);
void printOutputLine(EthernetFrame* frame);


int main(void)
{
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
    
    while(running)
    {

       //mvwprintw(title,1,1,"  size   ||        src          ||        dst         || type ||  proto");
        
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
        
        //build etherent output line
 
        char *ethernetString = NULL;
        ethernetString = buildEthernetOutput(&frame);
        
        if(ethernetString==NULL)
        {
            printf("error by return outputtext");
            return 1;
        
        }

        printf("%s",ethernetString); 
        
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
                char *flagTemp = malloc(sizeof flag);
                
                if(flagTemp == NULL)
                {
                    return 1;
                }
                packet->identifier = flagTemp;
                strcpy(packet->identifier,flag);

                
                //IPV4 Protocol - Type
                memcpy(&packet->protocol[0], &buffer[posIPV4 + IP_PROT_OFFSET], sizeof (packet->protocol[0]) * IP_PROT_LENGTH);

                //IPV$ Source Address 
                memcpy(&packet->src_ip[0], &buffer[posIPV4 + IP_SRC_OFFSET], sizeof (packet->src_ip[0]) * IP_SRC_LENGTH);
                //IPV$ dst Address 
                memcpy(&packet->dst_ip[0], &buffer[posIPV4 + IP_DST_OFFSET], sizeof (packet->dst_ip[0]) * IP_DST_LENGTH);

                // -> here print the specific part
            
                //allocated memory 
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

        printf("\n");
    }   
    return 0;
}


//prints the out a line in the window with ethernet
//TODO must be changed -> just prints the same by all types
void printOutputLine(EthernetFrame* frame/*, WINDOW* window*/)
{
 
    char outputText[256] = "";
    char outputTemp[256] = "";

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
        sprintf(outputTemp, "%2x", frame->type[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");

    //ip 
    IPV4 *packet= frame->nextLayer;
   
    //Protocol
    for(size_t i = 0; i < IP_PROT_LENGTH; i++)
    {
        sprintf(outputTemp, "%4i", packet->protocol[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //src addr 
    for(size_t i = 0; i < IP_SRC_LENGTH; i++)
    {
        sprintf(outputTemp, "%4d", packet->src_ip[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
     
    //dst addr 
    for(size_t i = 0; i < IP_DST_LENGTH; i++)
    {
        sprintf(outputTemp, "%4d", packet->dst_ip[i]);
        //printf("%s",outputTemp); 
        strcat(outputText, outputTemp);
    }
     
  
    strcat(outputText, "\n");
    printf("%s",outputText);

}

//this function generates just the string for the ethernet
char* buildEthernetOutput(EthernetFrame* frame)
{
    char outputText[256] = "";
    char outputTemp[256] = "";

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
        sprintf(outputTemp, "%2x", frame->type[i]);
        strcat(outputText, outputTemp);
    }
    char* ethernetOutput = malloc(strlen(outputText));

    if(ethernetOutput == NULL)
    {
        printf("error by malloc!");
        return "NULL";
    }
   
    strcpy(ethernetOutput,outputText);

    return ethernetOutput;
}

char* buildIPV4Output(EthernetFrame* frame)
{
    return NULL;
}