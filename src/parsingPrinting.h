#ifndef PARSINGPRINTING_H
#define PARSINGPRINTING_H

#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* buildEthernetOutput(EthernetFrame* frame);
char* buildIPv4Output(EthernetFrame* frame);
void printOutputLine(EthernetFrame* frame);
void parseBufferToFrame(EthernetFrame* frame, uint8_t* buffer);
void parseBufferToPacket(IPV4* packet, uint8_t* buffer);


#endif
