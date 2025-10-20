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
    // Clean up
    free(buffer);

    return 0;
}
