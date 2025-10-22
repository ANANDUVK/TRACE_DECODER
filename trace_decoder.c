/*
 * trace_decoder.c
 *
 * Definitions of N-Trace SYNC message parsing functions.
 * Author: ANANDU VK
 * Date: 2025-10-21
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int parse_function(const char *filename){
    FILE *fp = fopen(filename, "rb");  // Open file in binary mode

    // If open fails due to file doesn't exist or any other issues, retrun 1 and exit
    if (!fp) {
        printf("Error: Could not open trace file.\n");
        return 1;
    }

                  // Determine file size

    fseek(fp, 0, SEEK_END); //  Moves the file pointer to end of the file
    long int filesize = ftell(fp); // Return file size in bytes
    fseek(fp, 0, SEEK_SET); // Reset file pointer to start of file to start reading

    printf("Opened trace file  '%s', size: %ld bytes\n\n",filename, filesize);

    // Allocate buffer and read file
    uint8_t *buffer = malloc(filesize);
    if (!buffer) {
        printf("Error: Could not allocate buffer.\n");
        fclose(fp);
        return 1;
    }

    size_t read = fread(buffer, 1, filesize, fp);
    printf("Read %zu bytes from trace file\n\n\n\n", read);

    fclose(fp);

                                         // Now buffer contains the trace data, ready for parsing SYNC messages

                                         // Parsing N-Trace file stream byte-by-byte
    
     long i=0;   // Buffer Index 
     int flag=0;    // Flag used to indicate SYNC message detected and started
     long message_start=0;  // variable that hold  start address of  each SYNC message
     long message_end=0;    // variable that hold  end address of  each SYNC message
     int sync_count=0;  // Variable used to count no. of SYNC messages in the provided Trace file
     
     /* Bit stream used to store MDOs of each SYNC messages for further  parsing ( MDOs of every SYNC message bytes are padded with LSB first ).
      Store each 6 bits of chunks  in a bit container.
      From analysis length of each SYNC message is 14 , ie size=14 bytes (112 bits). So  minimum 112 Bits container required */
     unsigned __int128 bitstream = 0;   

     int bitpos = 0;     //   Bitposition in bitstream for parsing operations
     int icnt_end_pos = 0;  // Used to identify Interupt-counter end position. 
     int msg_end_pos = 0;   // Used to identify SYNC-message end position.
    //  int icnt_done = 0;

     while(i < filesize){

        uint8_t byte=buffer[i];
        uint8_t MDO=byte >>2;   // upper 6 bits of each byte
        uint8_t MSEO = byte & 0x03; // lower 2 bits of each byte


        // Detect start of any SYNC message in the data stream
        if(MDO==0x09 && MSEO==0){
            message_start=i;
            flag=1;    // flagged to indicate SYNC message started
            sync_count++; // Increment value for each SYNC message detection
            bitstream=0; // Reset bitstream for new SYNC messsages
            bitpos=0;   // Reset bitposition of bitstream for every new SYNC messages
            icnt_end_pos=0;  // Reset Instruction_counter end position in bitstream for every new SYNC message 
            printf("SYNC message start at offset 0x%04lX\n",message_start);

        }
        
        
        // Inside the SYNC message , MDOs of each bytes  appending to bitstream took place here
        if(flag){

            // append 6 bits of MDO, LSB-first
            bitstream |= ((unsigned __int128)MDO) << bitpos;

            bitpos += 6; // Increment bitposition in bitstream by 6 ( Because MDOs are of 6 bits in each byte)
            if(MSEO==0x01 && icnt_end_pos==0){  // (Safety) icnt_end_pos checked to avoid multiple assigning bitpos.
                icnt_end_pos=bitpos;   //  Bit position when variable length field ends
                
            }
        }

        

        // Detect  end of curent SYNC message. SYNC message end when MSEO =11 (0x03)
        if(flag && MSEO==0x03){
            message_end=i;
            printf("SYNC message end at offset 0x%04lx\n",message_end);
            printf("Length is %ld\n",message_end- message_start  +1);
            
            
            msg_end_pos=bitpos; //  Message end bit position 

            // Safety: if no 0x01 found in SYNC message , then I_CNT ends before message end
            if (icnt_end_pos == 0) {
                icnt_end_pos = msg_end_pos;
                                    }

            //extract fixed fields
            uint8_t TCODE =  bitstream & 0x3F;  // first 6 bits of each SYNC message is TCODE ( which is message header )
            uint8_t SRC  = (bitstream >> 6) & 0x0F;   // SRC include bits from 7th position to 10th position (4 bits)
            uint8_t SYNC = (bitstream >> 10) & 0x0F;  // SYNC includes bits from 11th position to 14th position (4 bits)
            uint8_t BTYPE = (bitstream >> 14) & 0x03;   // BTYPE includes bits from 15th position to 16th position (2 bits)

            /*                         Exctract variable length fields
               Variable length fields start after 16th position in bitstream ( because size of fixed legth fields already known (6+4+4+2))   */
            unsigned long long I_CNT = (unsigned long long)((bitstream >> 16) & (((__int128)1 << (icnt_end_pos - 16)) - 1));
            unsigned long long F_ADDR = (unsigned long long)((bitstream >> icnt_end_pos) & (((__int128)1 << (msg_end_pos - icnt_end_pos)) - 1));

            printf("Decoded fields of SYNC message %d is :\n", sync_count);
            printf("TCODE:0x%02X\n SRC:0x%0X\n SYNC:0x%0X\n BTYPE:0x%0X\n  I_COUNT:0x%llX\n  F_ADDR:0x%llX\n\n",TCODE, SRC, SYNC, BTYPE, I_CNT,F_ADDR);
            
            flag=0;    // Flagged to 0  indicate SYNC messsage ended
            bitstream=0;  // Reinitialize bitstream for next SYNC message
            bitpos=0;     // Reinitialize bitposition for next SYNC message
        }

        i++;   // Inceremnt buffer index for each bytes in buffer
              
     }
 printf("Total no. of SYNC messages in the file streams are : %d\n",sync_count);
 free(buffer); 
 return 0;
}

