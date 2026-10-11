#ifndef TYPES_H
#define TYPES_H


//Layer 3

typedef struct
{
    uint8_t protocol[2];
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    

} IPV4;

enum IPV
{
    IP_PROT_OFFSET     = 9,
    IP_PROT_LENGTH     = 1
};

//Layer 2

//TODO nextlayer als void pointer
typedef struct
{
    uint8_t dst[6];
    uint8_t src[6];
    uint8_t type[2];
    void *nextLayer;

} EthernetFrame;

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