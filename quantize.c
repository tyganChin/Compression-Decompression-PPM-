/*
 * Assignment: arith
 * Name: quantize.c
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/20/23
 * Summary: Quantizes/dequantizes the given pixels and saves them to a given 
 *          struct.
*/

#include <stdio.h>
#include <math.h>
#include "packInfo.h"
#include "codeWord.h"
#include "arith40.h"

/* shorten struct names */
#define quantizedVals struct quantizedVals
#define pixelVals struct pixelVals

/* number of coefficients */
const int NUM_COEFFICIENTS = 4;
const int NUM_COEFS_TO_RANGE_CHECK = 3;

/* quantization constants to scale the a coefficent */
const int A_QUANT_VALUE = ((uint64_t)(1 << A_WIDTH)) - 1;

/* quantization constant to scale b, c, and d coefficents */
const int BCD_QUANT_RANGE = (((uint64_t)(1 << B_WIDTH)) - 1);
const float PIXEL_RANGE = 0.6;
const int BCD_QUANT_VALUE = (BCD_QUANT_RANGE - 1) / PIXEL_RANGE;

/* upper and lower bounds for b, c, and d after quantization */
const int QUANT_UPPER_BOUND = BCD_QUANT_RANGE / 2;
const int QUANT_LOWER_BOUND = BCD_QUANT_RANGE / -2;

/* helper functions */
static void pixelToDCT(pixelVals *pVals);
static void quantizeCoefs(pixelVals *pVals, quantizedVals *qVals);
static void quantizeChroma(pixelVals *pVals, quantizedVals *qVals);
static void dequantizeChroma(quantizedVals *qVals, pixelVals *pVals);
static void dequantizeCoefs(quantizedVals *qVals, pixelVals *pVals);
static void DCTtoPixel(pixelVals *pVals);


/*******************************************************************************
*                            Public Functions                                  *
*******************************************************************************/

/*
 * Name: quantize
 * Purpose: Quantizes the pixel values held in the given pixelVals and saves 
 *          the quantized values in the given quantizedVals struct
 * Parameters: 
 *         pixelVals *pVals : A pointer to a struct that contains the pixel data
 *                            to be quantized
 *      quantizedVals *qVals : A pointer to the struct that stores the 
 *                             quantized values 
 * Output: n/a
 * Effects: The given quantizedVals struct is updated with the quantized values
 */
void quantize(pixelVals *pVals, quantizedVals *qVals)
{
        /* convert pixels to Discrete cosine space */
        pixelToDCT(pVals);

        /* quantize the coefficents */
        quantizeCoefs(pVals, qVals);

        /* quantize the chroma */
        quantizeChroma(pVals, qVals);
}

/*
 * Name: dequantize
 * Purpose: Dequantizes the quantized values held in the given struct and saves 
 *          the dequantized values in the given pixelVals struct
 * Parameters: 
 *      quantizedVals *qVals : A pointer to the struct that contians the 
 *                             quantized values 
 *          pixelVals *pVals : A pointer to a struct that stores the pixel data
 *                             unquantized.
 * Output: n/a
 * Effects: The given pixelVals struct is updated with the dequantized values
 */
void dequantize(quantizedVals *qVals, pixelVals *pVals)
{
        /* dequantize the chroma */
        dequantizeChroma(qVals, pVals);

        /* dequantize the coefficents */
        dequantizeCoefs(qVals, pVals);

        /* convert dequantized coefs from DCT to color space */
        DCTtoPixel(pVals);
}


/*******************************************************************************
*                         quantize Helper Functions                            *
*******************************************************************************/

/*
 * Name: pixelToDCT
 * Purpose: Convert the Y values in the given pixVals struct from pixel space  
 *          into discrete cosine space.
 * Parameters: 
 *      pixelVals *pVals : A pointer to a struct that stores the data contained 
 *                         in the given pixel array
 * Output: n/a
 * Effects: The Y vals in the pVA pixelVals struct are updated to DCT space
 */
void pixelToDCT(pixelVals *pVals)
{
        /* convert the Y values to cosine coefficients DCT*/
        float Y1 = pVals->Y1, Y2 = pVals->Y2, Y3 = pVals->Y3, Y4 = pVals->Y4;
        pVals->Y1 = (Y4 + Y3 + Y2 + Y1) / NUM_COEFFICIENTS;
        pVals->Y2 = (Y4 + Y3 - Y2 - Y1) / NUM_COEFFICIENTS;
        pVals->Y3 = (Y4 - Y3 + Y2 - Y1) / NUM_COEFFICIENTS;
        pVals->Y4 = (Y4 - Y3 - Y2 + Y1) / NUM_COEFFICIENTS;
}

/*
 * Name: quantizeCoefs
 * Purpose: Quantize the Y vals in the given pixVal struct and store the 
 *          quantized versions in the given quantizedVals struct
 * Parameters: 
 *          pixelVals *pVals : A pointer to a struct that stores the data 
 *                            contained in the given pixel array
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values of each DCT coefficent
 * Output: n/a
 * Effects: The 4 quantized coefficients are initialized in the quantizedVals 
 *          struct
 */
void quantizeCoefs(pixelVals *pVals, quantizedVals *qVals)
{
        /* quantize the DCT coefficients */
        qVals->qA = round(pVals->Y1 * A_QUANT_VALUE);
        qVals->qB = round(pVals->Y2 * BCD_QUANT_VALUE);
        qVals->qC = round(pVals->Y3 * BCD_QUANT_VALUE);
        qVals->qD = round(pVals->Y4 * BCD_QUANT_VALUE);

        /* make sure the b,c,d vals are betwen upper and lower bound */
        int64_t *quantVals[NUM_COEFS_TO_RANGE_CHECK];
        quantVals[0] = &qVals->qB;
        quantVals[1] = &qVals->qC;
        quantVals[2] = &qVals->qD;
        for (int i = 0; i < NUM_COEFS_TO_RANGE_CHECK; ++i) {
                if (*(quantVals[i]) > QUANT_UPPER_BOUND) {
                        *(quantVals[i]) = QUANT_UPPER_BOUND;
                } else if (*(quantVals[i]) < QUANT_LOWER_BOUND) {
                        *(quantVals[i]) = QUANT_LOWER_BOUND;
                }
        }
}

/*
 * Name: quantizeChroma
 * Purpose: Quantize the average pb and pr values and store the index in the 
 *          give quantizedVals struct
 * Parameters: 
 *          pixelVals *pVals : A pointer to a struct that stores the data 
 *                            contained in the given pixel array
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values of the pixel data
 * Output: n/a
 * Effects: The pbChroma and prChroma are initialized in the given 
 *          quantizedVals struct
 */
void quantizeChroma(pixelVals *pVals, quantizedVals *qVals)
{
        qVals->pbChroma = Arith40_index_of_chroma(pVals->pbAvg);
        qVals->prChroma = Arith40_index_of_chroma(pVals->prAvg);
}


/*******************************************************************************
*                        dequantize Helper Functions                           *
*******************************************************************************/

/*
 * Name: dequantizeChroma
 * Purpose: Dequantize the pb and pr chroma values and store the avg pb and pr  
 *          in the given pixelVals struct
 * Parameters: 
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values of the pixel data
 *      pixelVals *pVals : A pointer to a struct that stores the data 
 *                         unpacked word data
 * Output: n/a
 * Effects: The pbAvg and prAvg are initialized in the given pixelVals struct
 */
void dequantizeChroma(quantizedVals *qVals, pixelVals *pVals)
{
        pVals->pbAvg = Arith40_chroma_of_index(qVals->pbChroma);
        pVals->prAvg = Arith40_chroma_of_index(qVals->prChroma);
}

/*
 * Name: dequantizeCoef
 * Purpose: Dequantize the given quantized DCT coefficents and store the given 
 *          pixelVal struct
 * Parameters: 
 *      quantizedVals *qVals : A pointer to a struct that stores the quantized 
 *                             values of each DCT coefficent.
 *      pixelVals *pVals : A pointer to a struct that stores the data 
 *                         unpacked word data
 * Output: n/a
 * Effects: The given pixVal struct is updated with pixel space coefficents 
 */
void dequantizeCoefs(quantizedVals *qVals, pixelVals *pVals)
{
        pVals->Y1 = (float)qVals->qA / A_QUANT_VALUE;
        pVals->Y2 = (float)qVals->qB / BCD_QUANT_VALUE;
        pVals->Y3 = (float)qVals->qC / BCD_QUANT_VALUE;
        pVals->Y4 = (float)qVals->qD / BCD_QUANT_VALUE;
}  

/*
 * Name: pixelToDCT
 * Purpose: Convert the DCT coefficents in the given pixVals struct from 
 *          discrete cosine space to pixel space
 * Parameters: 
 *      pixelVals *pVals : A pointer to a struct that stores the data 
 *                         unpacked word data
 * Output: n/a
 * Effects: The y vals are updated
 */
void DCTtoPixel(pixelVals *pVals)
{
        /* save coefficinets to varaibles */
        float a = pVals->Y1, b = pVals->Y2, c = pVals->Y3, d = pVals->Y4;

        /* update Y vals be reversing the DCT */
        pVals->Y1 = a - b - c + d;
        pVals->Y2 = a - b + c - d;
        pVals->Y3 = a + b - c - d;
        pVals->Y4 = a + b + c + d;
}

#undef quantizedVals
#undef pixelVals
