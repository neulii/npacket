#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>


//Layer 3

//================         IPV4 Definition        ================



typedef struct
{
    uint8_t protocol[2];
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    

} IPV4;

//describes the positions and offsets of the fields
enum IPV4Data
{
    IP_PROT_OFFSET     = 9,
    IP_PROT_LENGTH     = 1
};



//================         ARP Definition        ================



typedef struct
{
    uint8_t protocol[2];
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    

} ARP;

//describes the positions and offsets of the fields
enum ARPData
{
    ARP_PROT_OFFSET     = 0,
    ARP_PROT_LENGTH     = 0
};


//==================================================================




//Layer 2  -> EthernetFrame

typedef struct
{
    ssize_t size;
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t type[2];
    void *nextLayer;

} EthernetFrame;

//describes the positions and offsets of the fields
enum EthernetData
{
    ETH_DST_OFFSET     = 0,
    ETH_SRC_OFFSET     = 6,
    ETH_TYPE_OFFSET    = 12,
    ETH_PAYLOAD_OFFSET = 14,
    ETH_DST_LENGTH     = 6,
    ETH_SRC_LENGTH     = 6,
    ETH_TYPE_LENGTH    = 2
};

#endif
