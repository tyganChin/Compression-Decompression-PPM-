/*
 * Assignment: arith
 * Name: compress40.c
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/18/2023
 * Summary: Compresses or decompress a given ppm file/compressed ppm file and 
 *          prints out the compressed ppm file/ decompressed file to stdout. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "compress40.h"
#include "codeWord.h"
#include "pack.h"
#include "RGBcompvConvert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "mem.h"
#include "pnm.h"
#include "assert.h"

/* used to help simplify the syntax */
typedef A2Methods_UArray2 A2;
#define Pmethods uarray2_methods_plain
#define Object A2Methods_Object

/* constants of the de/compression */
const int BLOCK_LENGTH = 2;
const int PIXEL_SIZE = 12;
const int WORD_BYTE_LENGTH = sizeof(codeWord);
const int BYTE = 8;
const int DENOMINATOR = 255;

/* used to iterate through a pixel block */
const int C[4] = {0, 1, 0, 1};
const int R[4] = {0, 0, 1, 1};

/* helper funcs */
static void RGBtoCV(int col, int row, A2 array2, Object *elem, void *den);
static A2 packPixmap(Pnm_ppm pixmap);
static void packPixel(int col, int row, A2 array2, Object *pix, void *pkdAr);
static void printBigEndian(int col, int row, A2 array2, Object *elem, void *cl);
static A2 makeWordArray(FILE *fp);
static void readWord(int col, int row, A2 array2, Object *elem, void *file);
static Pnm_ppm unpackPixmap(A2 packedImage);
static void unpackPixel(int col, int row, A2 array2, Object *pix, void *pkdAr);
static void CVtoRGB(int col, int row, A2 array2, Object *elem, void *den);


/*******************************************************************************
*                            Public Functions                                  *
*******************************************************************************/

/*
 * Name: compress40
 * Purpose: Compress each 2 by 2 block of pixels in the given ppm into 32 bit 
 *          words and print the compressed image to stdout.
 * Parameters: 
 *      FILE *fp : file pointer that contains the ppm file to compress
 * Output: n/a
 * Effects: The compressed file is printed to stdout row-major, in big endian 
 *          order as 32 bit compressed "words". Each of which represent 4 
 *          pixels in the original array.
 */
void compress40(FILE *fp)
{
        /* read in the file to reader and trim if needed */
        Pnm_ppm pixmap = Pnm_ppmread(fp, Pmethods);

        /* convert RGB color space to component video */
        Pmethods->map_row_major(pixmap->pixels, RGBtoCV, 
                                  &pixmap->denominator); 

        /* pack the bits into a 2d Uarray */
        A2 packed = packPixmap(pixmap);

        /* write pixelmap to stdout */
        int width = (pixmap->width / 2) * 2; /* ensures width is even */
        int height = (pixmap->height / 2) * 2; /* ensures height is even */
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        Pmethods->map_row_major(packed, printBigEndian, NULL);

        /* free the packed array and the pixelmap */
        Pnm_ppmfree(&pixmap);
        Pmethods->free(&packed);
}

/*
 * Name: decompress40
 * Purpose: Decompress the given file (32 bit words -> pixels) and print out 
 *          the decompressed ppm image to stdout
 * Parameters: 
 *      FILE *fp : A file pointer to the compressed ppm file 
 * Output: The decompressed file is written to stdout as a P6 ppm file
 */
void decompress40(FILE *fp)
{
        /* read in word image into Uarray */
        A2 packedImage = makeWordArray(fp);
        
        /* unpack image and populate a pixmap with it */
        Pnm_ppm pixmap = unpackPixmap(packedImage);

        /* convert image to RGB */
        Pmethods->map_row_major(pixmap->pixels, CVtoRGB, 
                                  &pixmap->denominator);

        /* write image to stdout */
        Pnm_ppmwrite(stdout, pixmap);

        /* free the packed image and the pixelmap */
        Pmethods->free(&packedImage);
        Pnm_ppmfree(&pixmap);
}


/*******************************************************************************
*                        Compression Helper Functions                          *
*******************************************************************************/

/*
 * Name: RGBtoCV
 * Purpose: Converts and rewrites every pixel in the pixelmap from RGB to 
 *          component video color space and save the converted pixel to
 * Parameters: 
 *           int col : Column of the current element in array
 *           int row : Row of the current element in array
 *         A2 array2 : The ppm image in RGB format
 *      Object *elem : The element at the current col/row in array
 *         void *den : The denominator of the pixmap needed for scaling
 * Output: n/a
 * Effects: The pixel in the given array is in CompV color space
 */
void RGBtoCV(int col, int row, A2 array2, Object *elem, void *den)
{
        /* void unused paramenters */
        (void) col;
        (void) row;
        (void) array2;
        
        /* perform swap */
        RGBtoCompV((int *)elem, (float *)elem, *(int *)den);
}

/*
 * Name: packPixmap
 * Purpose: Create, fill, and return an array of 32 bit "words" that each 
 *          represent a 2 by 2 block of pixels in the given pixel map.
 * Parameters: 
 *      Pnm_ppm pixmap : The pixelmap of the ppm image in CompV format
 * Output: An A2 array of bitpacked "words"
 * Note: Caller must free the returned A2 array (Pmethods->free())
 */
A2 packPixmap(Pnm_ppm pixmap)
{
        /* create uarray to hold packed words (omits odd width/height) */
        int width = floor(pixmap->width / BLOCK_LENGTH); 
        int height = floor(pixmap->height / BLOCK_LENGTH);
        A2 packed = Pmethods->new(width, height, WORD_BYTE_LENGTH);

        /* pack each 2 by 2 block in pixmap into packed array of words */
        Pmethods->map_row_major(packed, packPixel, pixmap->pixels);

        return packed;
}

/*
 * Name: packPixel
 * Purpose: Pack the comp video values of the current 2 by 2 block of pixels 
 *          into a single 32 bit word in the given packedArray. 
 * Parameters: 
 *            int col : Column of the current element in array
 *            int row : Row of the current element in array
 *           A2 pkdAr : The array of packed words
 *        Object *pix : The element at the current col/row in the pixmap
 *       void *pixels : The pixelmap
 * Output: n/a
 * Effects: A "word" is added to the packedArray
 */
void packPixel(int col, int row, A2 pkdAr, Object *word, void *pixels)
{
        /* void unused parameter */
        (void) pkdAr;

        /* cast pixel array and set it's current col and row */
        A2 pixls = (A2)pixels;
        int pCol = col * BLOCK_LENGTH, pRow = row * BLOCK_LENGTH;

        /* get the block of pixels to compress */
        float *pix[WORD_BYTE_LENGTH];
        for (int i = 0; i < WORD_BYTE_LENGTH; ++i) {
                pix[i] = (float *)Pmethods->at(pixls, pCol + C[i], pRow + R[i]);
        }

        /* get the element that will store the compressed data */
        codeWord *pkdWord = (codeWord *)word;

        /* pack the 4 pixels into the 32 bit word */
        packWord(pix, pkdWord);
}

/*
 * Name: printBigEndian
 * Purpose: Print out the current element to stdout in big endian order
 * Parameters: 
 *           int col : Column of the current element in array
 *           int row : Row of the current element in array
 *         A2 array2 : The array of packed words
 *      Object *elem : The element at the current col/row in array
 *          void *cl : NULL
 * Output: n/a
 * Effects: The curr elem is printed out most signifcant byte to least
 */
void printBigEndian(int col, int row, A2 array2, Object *elem, void *cl)
{
        /* unused parameters */
        (void) col;
        (void) row;
        (void) array2;
        (void) cl;

        /* print in big endian order */
        char *bytes = (char *)elem;
        for (int i = WORD_BYTE_LENGTH - 1; i >= 0; --i) {
                putchar(bytes[i]);
        }
}


/*******************************************************************************
*                        Decompression Helper Functions                        *
*******************************************************************************/

/*
 * Name: makeWordArray
 * Purpose: Read the packed "words" in the given file into a Uarray
 * Parameters: 
 *      FILE *fp : The compressed file to read from
 * Output: The array of packed words
 * Notes: The array must be freed by the caller (Pmethods->free()) 
 * Expectations: The given file pointer is formatted correctly. CRE if not.
 */
A2 makeWordArray(FILE *fp)
{
        /* read in header */
        unsigned height, width;
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        /* initialze packed word array */
        A2 wordArray = Pmethods->new(width / BLOCK_LENGTH, 
                                     height / BLOCK_LENGTH, 
                                     PIXEL_SIZE);

        /* read in words into array */
        Pmethods->map_row_major(wordArray, readWord, fp);

        /* return array */
        return wordArray;
}

/*
 * Name: readWord
 * Purpose: Read a single word from the file into the current element in the 
 *          array
 * Parameters: 
 *           int col : Column of the current element in array
 *           int row : Row of the current element in array
 *         A2 array2 : The array to read the next word of the file into
 *      Object *elem : The current element in the array
 *          void *fp : The open file of packed words
 * Output: n/a
 * Effects: The next word in the file is read into the current element in the 
 *          array
 * Expectations: The given file pointer is the same length as the packed array. 
 *               CRE if not.
 */
void readWord(int col, int row, A2 array2, Object *elem, void *file)
{
        /* void unused parameters */
        (void) col;
        (void) row;
        (void) array2;

        /* read the next word of the file in */
        FILE *fp = (FILE *)file;
        codeWord currWord;
        assert(fread(&currWord, sizeof(codeWord), 1, fp) == 1);

        /* cast word and set it to 0 */
        codeWord *word = (codeWord *)elem;
        *word = 0;

        /* add the word read in to the word in array in little endian order */
        for (int i = 0; i < WORD_BYTE_LENGTH; ++i) {

                /* Left-shift to make room for the next byte */
                *word <<= BYTE;

                /* Extract the byte and add it to word */
                *word |= (currWord & (((uint64_t)(1 << BYTE)) - 1)); 
                
                /* Right-shift currWord to process the next bit */
                currWord >>= BYTE;
        }
}

/*
 * Name: unpackPixmap
 * Purpose: Unpack the words in the given array into a pixelmap struct 
 *          containing a pixel map of CompV pixels. (1 word = 4 pixels)
 * Parameters: 
 *      A2 packedImage : The array of bit packed words representing the 
 *                       compressed pnm file.
 * Output: A Pnm_ppm object containing the unpacked words
 * Notes: The pixmap must be freed by the caller (Pnm_ppmfree()) 
 */
Pnm_ppm unpackPixmap(A2 packedImage)
{
        /* declare new pixelmap to store the unpacked image */
        Pnm_ppm pixmap;
        NEW(pixmap);

        /* initialize the pixmap with an array double the size of packedImage */
        int width = Pmethods->width(packedImage) * BLOCK_LENGTH;
        int height = Pmethods->height(packedImage) * BLOCK_LENGTH;
        pixmap->width = width, pixmap->height = height;
        pixmap->denominator = DENOMINATOR, pixmap->methods = Pmethods;
        pixmap->pixels = Pmethods->new(width, height, PIXEL_SIZE);

        /* unpack image into pixmap array */
        Pmethods->map_row_major(pixmap->pixels, unpackPixel, packedImage);

        /* return pixmap */
        return pixmap;
}

/*
 * Name: unpackPixel
 * Purpose: Unpack the current word in the array of packed words and store the 
 *          4 resulting pixels in the current element
 * Parameters: 
 *            int col : Column of the current element in array
 *            int row : Row of the current element in array
 *          A2 pixels : The pixel map that stores the converted words
 *        Object *pix : The current pixel in the array
 *        void *pkdAr : The array of bitpacked "words" to unpack
 * Output: n/a
 * Effects: Current word is unpacked and put into the current 2 by 2 block of 
 *          pixels
 */
void unpackPixel(int col, int row, A2 pixels, Object *pix, void *pkdAr)
{
        /* void unused parameter */
        (void) pix;

        /* check if current position is begining of the block */
        if (col % BLOCK_LENGTH != 0 || row % BLOCK_LENGTH != 0) {
                return;
        }

        /* get the element that will store the compressed data */
        A2 packed = (A2)pkdAr;
        codeWord word = *(codeWord *)Pmethods->at(packed, col / BLOCK_LENGTH, 
                                                  row / BLOCK_LENGTH);

        /* get the block of pixels to compress */
        float *pixs[WORD_BYTE_LENGTH];
        for (int i = 0; i < WORD_BYTE_LENGTH; ++i) {
                pixs[i] = (float *)Pmethods->at(pixels, col + C[i], row + R[i]);
        }

        /* pack the 4 pixels into the 32 bit word */
        unpackWord(word, pixs);
}

/*
 * Name: CVtoRGB
 * Purpose: Converts, then overwritesthe element from component video to RGB 
 *          color space
 * Parameters: 
 *           int col : Column of the current element in array
 *           int row : Row of the current element in array
 *         A2 array2 : An array of pixels in Comp Video color space
 *      Object *elem : The current element in the array
 *         void *den : The denominator of the pixmap needed for scaling
 * Output: n/a
 * Effects: The element is in RGB color space.
 */
void CVtoRGB(int col, int row, A2 array2, Object *elem, void *den)
{
        /* void unused paramenters */
        (void) col;
        (void) row;
        (void) array2;
        
        /* perform swap */
        CompVtoRGB((float *)elem, (int *)elem, *(int *)den);
}

#undef Pmethods
#undef Object
