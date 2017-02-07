#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>

int main(int argc, char *argv[]){
    
    if (argc!=2) {
        fprintf(stderr, "Usage: ./recover raw_file\n");
        return 1;
    }
    
    char *infile = argv[1];
    
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }
    
    unsigned char *buffer = malloc(512*sizeof(char));
    
    bool jpegFound = false;
    int jpegCount = 0;
    char *filename;
    filename = malloc(32);
    FILE *img = NULL;
    
    //printf("fread returns: %zu\n",fread(&buffer, 512, 1, inptr));
    //printf("JPEG Not Found\tFirst 4 bytes: %i %i %i %i\n", (int)buffer[0],(int)buffer[1],(int)buffer[2],(int)buffer[3]);
    
    while (fread(buffer, 1, 512, inptr) == 512) {
        
        if (buffer[0]==0xff && buffer[1]==0xd8 && buffer[2]==0xff && (buffer[3]&0xf0) == 0xe0) {
            
            jpegFound = true;
            // printf("JPEG Found:\tFirst 4 bytes: %#02x %#02x %#02x %#02x\n", buffer[0],buffer[1],buffer[2],buffer[3]);
            
            if (img!=NULL) fclose(img);
            
            sprintf(filename, "%03i.jpg", jpegCount);
            jpegCount++;
            
            img = fopen(filename, "w");
            
            if (img == NULL) {
                fprintf(stderr, "Could not create %s.\n", filename);
                return 2;
            }
        }
        
        if (jpegFound){    
            fwrite(buffer, 1, 512, img);
        }
        
    }
    
    free(buffer);
    free(filename);
    fclose(img);
    fclose(inptr);
    return 0;
}