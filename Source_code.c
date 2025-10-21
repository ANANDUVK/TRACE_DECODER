#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    FILE *fp = fopen("ntracedata.rtd", "rb");  // Open file in binary mode
    if (!fp) {
        printf("Error: Could not open trace file.\n");
        return 1;
    }

    // Determine file size
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    printf("Opened trace file, size: %ld bytes\n", filesize);

    // Allocate buffer and read file
    uint8_t *buffer = malloc(filesize);
    if (!buffer) {
        printf("Error: Could not allocate buffer.\n");
        fclose(fp);
        return 1;
    }

    size_t read = fread(buffer, 1, filesize, fp);
    printf("Read %zu bytes from trace file\n", read);

    fclose(fp);

    // Now buffer contains the trace data, ready for parsing SYNC messages!  

     // Parsing N-Trace file stream byte-by-byte
    
     long i=0;
     int flag=0;
     long message_start=0;
     long message_end=0;
     int sync_count=0;
     unsigned __int128 bitstream = 0;
     int bitpos = 0;


     while(i < filesize){

        uint8_t byte=buffer[i];
        uint8_t MDO=byte >>2;   // upper 6 bits of each byte
        uint8_t MSEO = byte & 0x03; // lower 2 bits of each byte


        // Detect start of any SYNC message in the data stream
        if(MDO==0x09 && MSEO==0){
            message_start=i;
            flag=1;
            sync_count++;
            bitstream=0;
            bitpos=0;
            printf("SYNC message start at offset 0x%04lx\n",message_start);

            //appending first chunk 
            bitstream |= ((unsigned __int128)MDO) << bitpos;
            bitpos += 6;
        }
        
        
        //  Combining all 6-bit chunks (MDO of each Bytes of a SYNC message) into one continous bitstream of 64 buts
        else if(flag){
             // append 6 bits of MDO, LSB-first
            bitstream |= ((unsigned __int128)MDO) << bitpos;
            bitpos += 6;
        }
                
        // Detect  end of curent SYNC message
        if(flag && MSEO==3){
            message_end=i;
            printf("SYNC message end at offset 0x%04lx\n",message_end);
            printf("Length is %ld\n",message_end- message_start  +1);
            

            //extract fixed fields
            uint8_t TCODE = bitstream & 0x3F;       // first 6 bits of each SYNC message
            uint8_t SRC   = (bitstream >> 6) & 0x0F;
            uint8_t SYNC  = (bitstream >> 10) & 0x0F;
            uint8_t BTYPE = (bitstream >> 14) & 0x03;

            printf("Decoded fields of SYNC message %d is :\n", sync_count);
            printf("TCODE:0x%0X\n SRC:0x%0X\n SYNC:0x%0X\n BTYPE:0x%0X\n\n",TCODE, SRC, SYNC, BTYPE);
            
            flag=0;
            bitstream=0;
            bitpos=0;
        }

        i++;
              
     }
 printf("Total no. of SYNC messages in the file streams are : %d\n",sync_count);
 free(buffer);
 return 0;
}
