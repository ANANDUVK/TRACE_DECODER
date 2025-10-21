/*
 * trace_decoder.h
 *
 * Declarations for N-Trace SYNC message decoder functions.
 * Author: ANANDU VK
 * Date: 2025-10-21
 */

#ifndef TRACE_DECODER_H  
#define TRACE_DECODER_H

#include <stdint.h>

// Parse N-Trace file(.rtd file) for SYNC messages, returns 0 on success
int parse_function(const char *filename);

#endif 
