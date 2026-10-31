#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <unistd.h>
#include <time.h>


//Layer 3

//================         IPV4 Definition        ================

//describes the positions and offsets of the fields
enum IPV4Data
{
    IP_LENGTH_OFFSET   = 2,     //Total Length of the IP-Packet
    IP_TTL_OFFSET      = 8,     //Time To Live 
    IP_PROT_OFFSET     = 9,     //Protocol -> UDP/TCP
    IP_SRC_OFFSET      = 12,    //Souce IP
    IP_DST_OFFSET      = 16,    //Destination IP

    IP_LENGTH_LENGTH   = 2,
    IP_TTL_LENGTH      = 1, 
    IP_PROT_LENGTH     = 1,
    IP_SRC_LENGTH      = 4,
    IP_DST_LENGTH      = 4
};

typedef struct
{
    
    char *identifier;   //just for identifying
    uint8_t length[IP_LENGTH_LENGTH];
    uint8_t ttl[IP_TTL_LENGTH];    
    uint8_t protocol[IP_PROT_LENGTH];
    uint8_t src_ip[IP_SRC_LENGTH];
    uint8_t dst_ip[IP_DST_LENGTH];
    
} IPV4;


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

//describes the positions and offsets of the fields
enum EthernetData
{
    ETH_DST_OFFSET     = 0,
    ETH_SRC_OFFSET     = 6,
    ETH_TYPE_OFFSET    = 12,     
    ETH_PAYLOAD_OFFSET = 14,    //The Offset where the Payload begins
   
    ETH_DST_LENGTH     = 6,
    ETH_SRC_LENGTH     = 6,
    ETH_TYPE_LENGTH    = 2
};

typedef struct
{
    ssize_t size;
    struct timespec timestamp;
    long frameNr;
   
    uint8_t dst[ETH_DST_LENGTH];
    uint8_t src[ETH_SRC_LENGTH];
    uint8_t type[ETH_TYPE_LENGTH];
   
    void *nextLayer;

} EthernetFrame;

#endif
