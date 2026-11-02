#include "parsingPrinting.h"


char* buildEthernetOutput(EthernetFrame* frame)
{
    char outputText[256] = "";
    char outputTemp[256] = "";

    //Framecounter

    sprintf(outputTemp, "%4ld", frame->frameNr);
    strcat(outputText, outputTemp); 

    strcat(outputText, " || ");

    //TimeStamp
    double timest = frame->timestamp.tv_sec + frame->timestamp.tv_nsec/1000000000.0;

    sprintf(outputTemp, "%5.5f", timest);
    strcat(outputText, outputTemp);

    strcat(outputText, " || ");

    //size
    sprintf(outputTemp, "%5li  ||  ", frame->size);
    strcat(outputText, outputTemp);

    //dst mac
    for(size_t i = 0; i < ETH_DST_LENGTH; i++)
    {
        sprintf(outputTemp, "%02x ", frame->dst[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //src mac
    for(size_t i = 0; i < ETH_SRC_LENGTH; i++)
    {
        sprintf(outputTemp, "%02x ", frame->src[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //ethertype
    for(size_t i = 0; i < ETH_TYPE_LENGTH; i++)
    {
        sprintf(outputTemp, "%02x", frame->type[i]);
        strcat(outputText, outputTemp);
    }
    
    char* ethernetOutput = malloc(strlen(outputText) + 1);

    if(ethernetOutput == NULL)
    {
        printf("error by malloc!");
        return NULL;
    }
   
    strcpy(ethernetOutput,outputText);

    return ethernetOutput;
}

//LEGACY
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
char* buildIPv4Output(EthernetFrame* frame)
{
    char outputText[256] = "";
    char outputTemp[256] = "";

    //ip 
    IPV4 *packet= frame->nextLayer;

    //Total Length
    //calculate to bigger datatype -> from 2x2 bytes to 1x4 bytes
    int16_t ip_length = ((uint16_t)packet->length[0] << 8) | packet->length[1];

    sprintf(outputTemp, "%4i", ip_length);
    strcat(outputText, outputTemp);

    strcat(outputText, " || ");
   
    //TTL
    for(size_t i = 0; i < IP_TTL_LENGTH; i++)
    {
        sprintf(outputTemp, "%3i", packet->ttl[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");

    //Protocol
    for(size_t i = 0; i < IP_PROT_LENGTH; i++)
    {
        sprintf(outputTemp, "%2i", packet->protocol[i]);
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
    
    //src addr 
    for(size_t i = 0; i < IP_SRC_LENGTH; i++)
    {
        sprintf(outputTemp, "%3d", packet->src_ip[i]);
        if(i < IP_SRC_LENGTH-1)
        {
            strcat(outputTemp, ".");
        }
        strcat(outputText, outputTemp);
    }
    
    strcat(outputText, " || ");
     
    //dst addr 
    for(size_t i = 0; i < IP_DST_LENGTH; i++)
    {
        sprintf(outputTemp, "%3d", packet->dst_ip[i]);
        if(i < IP_DST_LENGTH-1)
        {
            strcat(outputTemp, ".");
        }

        //printf("%s",outputTemp); 
        strcat(outputText, outputTemp);
    }
     
    char* IPv4Output = malloc(strlen(outputText) + 1);

    if(IPv4Output == NULL)
    {
        printf("error by malloc!");
        return NULL;
    }
   
    strcpy(IPv4Output,outputText);

    return IPv4Output;
}

void parseBufferToFrame(EthernetFrame* frame, uint8_t* buffer)
{
    
 //Destination Mac
    memcpy(&frame->dst[0], &buffer[ETH_DST_OFFSET],sizeof(frame->dst[0]) * ETH_DST_LENGTH);

    //Source Mac
    memcpy(&frame->src[0], &buffer[ETH_SRC_OFFSET],sizeof (frame->src[0]) * ETH_SRC_LENGTH);
    
    //Ethertype
    memcpy(&frame->type[0], &buffer[ETH_TYPE_OFFSET],sizeof (frame->type[0])*ETH_TYPE_LENGTH);
}

void parseBufferToPacket(IPV4* packet, uint8_t* buffer)
{
    //Total Length
    memcpy(&packet->length[0], &buffer[ETH_PAYLOAD_OFFSET + IP_LENGTH_OFFSET], sizeof (packet->length[0]) * IP_LENGTH_LENGTH);

   //TTL
    memcpy(&packet->ttl[0], &buffer[ETH_PAYLOAD_OFFSET + IP_TTL_OFFSET], sizeof (packet->ttl[0]) * IP_TTL_LENGTH);

    //IPV4 Protocol - Type
    memcpy(&packet->protocol[0], &buffer[ETH_PAYLOAD_OFFSET + IP_PROT_OFFSET], sizeof (packet->protocol[0]) * IP_PROT_LENGTH);

    //IPV$ Source Address 
    memcpy(&packet->src_ip[0], &buffer[ETH_PAYLOAD_OFFSET + IP_SRC_OFFSET], sizeof (packet->src_ip[0]) * IP_SRC_LENGTH);
    //IPV$ dst Address 
    memcpy(&packet->dst_ip[0], &buffer[ETH_PAYLOAD_OFFSET + IP_DST_OFFSET], sizeof (packet->dst_ip[0]) * IP_DST_LENGTH);

}
