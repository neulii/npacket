#ifndef TYPES_H
#define TYPES_H


//Layer 3

typedef struct
{
    uint8_t protocol[2];
    uint8_t src_ip[4];
    uint8_t dst_ip[4];

} IPV4;

uint8_t posProto =     9;      uint8_t lProto =  1;
//uint8_t posSrcMac =     6;      uint8_t lSrcMac =       6;
//uint8_t posEthertype =  12;     uint8_t lEthertype =    2;

//Layer 2

typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t type[2];
    IPV4 *nextLayer;

} EthernetFrame;

//Offset of Datatypes in frame:
uint8_t posDstMac    = 0;      uint8_t lDstMac =       6;
uint8_t posSrcMac    = 6;      uint8_t lSrcMac =       6;
uint8_t posEthertype = 12;     uint8_t lEthertype =    2;
uint8_t posPayload   = 14;
    

#endif