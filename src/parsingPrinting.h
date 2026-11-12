#ifndef PARSINGPRINTING_H
#define PARSINGPRINTING_H

#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* buildEthernetOutput(EthernetFrame* frame);
char* buildIPv4Output(EthernetFrame* frame);
void printOutputLine(EthernetFrame* frame);
void parseBufferToFrame(EthernetFrame* frame, const uint8_t* buffer);
void parseBufferToPacket(IPV4* packet, const uint8_t* buffer);


#endif
