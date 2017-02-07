/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    } else if (argv[1]<0) {
        fprintf(stderr, "Usage: ./resize n infile outfile\n n must be a positive floating point number\n");
        return 1;
    }

    // remember filenames and save resize factor
    float resFactor = atof(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // save old values, write new ones    
    int oldPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int oldHeight = bi.biHeight;
    int oldWidth = bi.biWidth;
    int newHeight = round(bi.biHeight*resFactor);
    int newWidth = round(bi.biWidth*resFactor);
    bi.biHeight = newHeight;
    bi.biWidth = newWidth;
    
    // determine updated padding for scanlines and biSizeImage
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    bi.biSizeImage = abs(newHeight) * ( (newWidth * sizeof(RGBTRIPLE)) + padding);
    
    // modify and write outfile's BITMAPFILEHEADER
    bf.bfSize = (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bi.biSizeImage);
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // save input scanline temporary storage
    RGBTRIPLE triple[abs(oldHeight)][oldWidth];
    
    //get each scanline from infile
    for (int i = 0; i < abs(oldHeight); i++)
    {    
        // iterate over pixels in scanline
        for (int j = 0; j < oldWidth; j++)
        {
            // read RGB triple from infile
            fread(&triple[i][j], sizeof(RGBTRIPLE), 1, inptr);
        }
        // skip over padding, if any
        fseek(inptr, oldPadding, SEEK_CUR);
    }
    // write each scanline on outfile
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // write expanded scanlines from old to resized
        for (int j = 0; j < bi.biWidth; j++)
        {
            int sourceIndexH = i/resFactor;
            int sourceIndexW = j/resFactor;
            fwrite(&triple[sourceIndexH][sourceIndexW], sizeof(RGBTRIPLE), 1, outptr);
        }

        // add padding
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
