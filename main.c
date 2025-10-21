/*
 * main.c
 *
 * Entry point for N-Trace SYNC decoder.
 * Author: ANANDU VK
 * Date: 2025-10-21
 */



 
#include <stdio.h>
#include "trace_decoder.h"

int main(void)
{
    printf("============================================\n");
    printf("   RISC-V N-TRACE SYNC Message Decoder\n");
    printf("============================================\n");

    parse_function("ntracedata.rtd");

    printf("\nDecoding complete\n");
    printf("============================================\n");
    return 0;
}