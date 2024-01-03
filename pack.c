/*
 * Assignment: arith
 * Name: pack.c
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/18/2023
 * Summary: Pack the given 2 by 2 block of Component Video pixels into a 32 bit 
 *          bitpacked word or unpacks a 32 bitpacked word into a 2 by 2 block 
 *          of Component Video pixels. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pack.h"
#include "bitpack.h"
#include "codeWord.h"
#include "packInfo.h"
#include "quantize.h"
#include "assert.h"

/* shorten struct names */
#define quantizedVals struct quantizedVals
#define pixelVals struct pixelVals

/* number of pixels to pack */
const int NUMPIX = 4;

/* helper functions */
static void pullOutPixVals(float **pix, pixelVals *pVals);
static void putInQVals(codeWord *word, quantizedVals *qVals);
static void pullOutQVals(codeWord word, quantizedVals *qVals);
static void putInPixVals(float **pix, pixelVals *pVals);


/*******************************************************************************
*                            Public Functions                                  *
*******************************************************************************/

/*
 * Name: packWord
 * Purpose: Pack the given array of 4 pixels into the given 32 bit word by 
 *          compressing the data
 * Parameters: 
 *         float **pix : An array of pointers to 4 float values that each 
 *                       represent a pixel that is to be packed into the word
 *      codeWord *word : A pointer to the a 32 bit word to pack the compressed 
 *                       pixels into
 * Output: n/a
 * Effects: Pixel data is compressed into the word passed in by caller
 */
void packWord(float **pix, codeWord *word)
{
        /* declare the structs to store pixel vals and quantized vals */
        pixelVals pVals;
        quantizedVals qVals;

        /* intialize pixel values struct */
        pullOutPixVals(pix, &pVals);

        /* quantize the pixel values and put them into q val struct */
        quantize(&pVals, &qVals);

        /* pack the quanitzed vals in qVals into given word */
        putInQVals(word, &qVals);
}

/*
 * Name: unpackWord
 * Purpose: Unpack the given bit packed word into 4 component video pixels and 
 *          save the information in each of those pixels to the given pixel 
 *          array
 * Parameters:
 *      codeWord word : The 32 bit, bit-packed word that is to be unpacked
 *        float **pix : An array of 4 pixels to store the unpacked information 
 *                      in
 * Output: n/a
 * Effects: The given pix array is updated with the unpacke data through a 
 *          pointer in the function that calls this
 */
void unpackWord(codeWord word, float **pix)
{
        /* declare the structs to store pixel vals and quantized vals */
        quantizedVals qVals;
        pixelVals pVals;

        /* intialize quantized values struct */
        pullOutQVals(word, &qVals);

        /* dequantize the pixel values and put them into pix val struct */
        dequantize(&qVals, &pVals);

        /* put the unquanitzed vals in pVals into given word */
        putInPixVals(pix, &pVals);
} 


/*******************************************************************************
*                         packWord Helper Functions                            *
*******************************************************************************/

/*
 * Name: pullOutPixVals
 * Purpose: Initialzes the given pixelVal struct using the given pixel values 
 *          (Y, pr, and pb values)
 * Parameters: 
 *           float **pix : An array of pointers to 4 float values that each 
 *                         represent a pixel 
 *      pixelVals *pVals : A pointer to a struct that stores the data contained 
 *                         in the given pixel array
 * Output: n/a
 * Effects: The given pixelValue struct is initialized to hold the information 
 *          stored in the given pix array.
 */
void pullOutPixVals(float **pix, pixelVals *pVals)
{
        /* assign the 4 Y vals to struct */
        pVals->Y1 = pix[0][0];
        pVals->Y2 = pix[1][0];
        pVals->Y3 = pix[2][0];
        pVals->Y4 = pix[3][0];

        /* calcuate average pb and pr values among the 4 pixels */
        pVals->pbAvg = (pix[0][1] + pix[1][1] + pix[2][1] + pix[3][1]) / NUMPIX;
        pVals->prAvg = (pix[0][2] + pix[1][2] + pix[2][2] + pix[3][2]) / NUMPIX;
}

/*
 * Name: putInQVals
 * Purpose: Pack the quantized values in the quantizedVals struct into the 
 *          given word 
 * Parameters: 
 *            code *word : A pointer to the word that will store the packed 
 *                             quantized pixel data.
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values of each DCT coefficent
 * Output: n/a
 * Effects: The word is fully packed witht the quantized data
 */
void putInQVals(codeWord *word, quantizedVals *qVals)
{
        *word = Bitpack_newu(*word, A_WIDTH, A_LSB, qVals->qA);
        *word = Bitpack_news(*word, B_WIDTH, B_LSB, qVals->qB);
        *word = Bitpack_news(*word, C_WIDTH, C_LSB, qVals->qC);
        *word = Bitpack_news(*word, D_WIDTH, D_LSB, qVals->qD);
        *word = Bitpack_newu(*word, PB_WIDTH, PB_LSB, qVals->pbChroma);
        *word = Bitpack_newu(*word, PR_WIDTH, PR_LSB, qVals->prChroma);
}

      
/*******************************************************************************
*                        unpackWord Helper Functions                           *
*******************************************************************************/

/*
 * Name: pullOutQVals
 * Purpose: Initialzes the given quantizedVal struct using given word
 * Parameters: 
 *             codeWord word : The word that contians the packed quantized 
 *                             pixel data
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values
 * Output: n/a
 * Effects: The given quantizedVal struct is initialized to hold the 
 *          information stored in the given bit packed word.
 */
void pullOutQVals(codeWord word, quantizedVals *qVals)
{
        qVals->qA = Bitpack_getu(word, A_WIDTH, A_LSB);
        qVals->qB = Bitpack_gets(word, B_WIDTH, B_LSB);
        qVals->qC = Bitpack_gets(word, C_WIDTH, C_LSB);
        qVals->qD = Bitpack_gets(word, D_WIDTH, D_LSB);
        qVals->pbChroma = Bitpack_getu(word, PB_WIDTH, PB_LSB);
        qVals->prChroma = Bitpack_getu(word, PR_WIDTH, PR_LSB);
}

/*
 * Name: putInPixVals
 * Purpose: Put the unpacked pixel data in the pixelVals struct into the 
 *          given array of pixels
 * Parameters: 
 *           float **pix : An array of 4 pixels to store the unpacked 
 *                          information in
 *      pixelVals *pVals : A pointer to a struct that stores the data 
 *                         unpacked word data
 * Output: n/a
 * Effects: The word is fully packed witht the quantized data
 */
void putInPixVals(float **pix, pixelVals *pVals)
{
        /* assign y vals */
        pix[0][0] = pVals->Y1;
        pix[1][0] = pVals->Y2;
        pix[2][0] = pVals->Y3;
        pix[3][0] = pVals->Y4;
        
        /* assing pb and pr averages to each pixel */
        pix[0][1] = pix[1][1] = pix[2][1] = pix[3][1] = pVals->pbAvg;
        pix[0][2] = pix[1][2] = pix[2][2] = pix[3][2] = pVals->prAvg;
}

#undef quantizedVals
#undef pixelVals