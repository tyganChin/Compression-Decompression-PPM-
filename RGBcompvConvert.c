/*
 * Assignment: arith
 * Name: RGBcompvConvert.c
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/18/2023
 * Summary: Converts the given pixel either from RGB color space to Component 
 *          Video space or Component Video space to RGB.
 */

#include "RGBcompvConvert.h"
#include "assert.h"
#include <math.h>

/* upper and lower bounds for component video */
const int Y_UPPERBOUND = 1;
const int Y_LOWERBOUND = 0;
const float PB_PR_UPPERBOUND = 0.5;
const float PB_PR_LOWERBOUND = -0.5;

/* lower bound for RGB */
const int RGB_LOWERBOUND = 0;

/* RGB -> CompV conversion constants */
const float Y_RED = 0.299, Y_GREEN = 0.587, Y_BLUE = 0.114;
const float PB_RED = -0.168736, PB_GREEN = 0.331264, PB_BLUE = 0.5;
const float PR_RED = 0.5, PR_GREEN = 0.418688, PR_BLUE = 0.081312;

/* CompV -> RGV conversion constants */
const float RED_Y = 1.0, RED_PB = 0.0, RED_PR = 1.402;
const float GREEN_Y = 1.0, GREEN_PB = 0.344136, GREEN_PR = 0.714136;
const float BLUE_Y = 1.0, BLUE_PB = 1.772, BLUE_PR = 0.0;

/*
 * Name: RGBtoCompV
 * Purpose: Convert the given RGB pixel to Component video respresentation and 
 *          save the new components in the given component video pixel.
 * Parameters: 
 *          int *RGBpixel : A pointer to an integer array containing the pixels 
 *                          red green and blue values
 *      float *CompVpixel : A pointer to a float array where the converted RGB 
 *                          values will go.
 *        int denominator : The max val of the ppm image the RGB pixel came 
 *                          from. Used to scale the values.
 * Output: n/a
 * Effects: The given CompVpixel array is updated with the corresponding Y, Pb, 
 *          and Pr values
 */
void RGBtoCompV(int *RGBpixel, float *CompVpixel, int denominator)
{
        /* get the RGB values */
        float red = RGBpixel[0], green = RGBpixel[1], blue = RGBpixel[2];
        
        /* scale them based on the denomonator */
        red /= denominator;
        green /= denominator;
        blue /= denominator;

        /* convert each value stored int the element R G B to y pb and pr */
        float y = (Y_RED * red) + (Y_GREEN * green) + (Y_BLUE * blue);
        float pb = (PB_RED * red) - (PB_GREEN * green) + (PB_BLUE * blue);
        float pr = (PR_RED * red) - (PR_GREEN * green) - (PR_BLUE * blue);

        /* ensure vals stay in range */
        y = fmin(Y_UPPERBOUND, fmax(Y_LOWERBOUND, y));
        pb = fmin(PB_PR_UPPERBOUND, fmax(PB_PR_LOWERBOUND, pb));
        pr = fmin(PB_PR_UPPERBOUND, fmax(PB_PR_LOWERBOUND, pr));

        /* swap the RGB values with the new video component values */
        CompVpixel[0] = y;
        CompVpixel[1] = pb;
        CompVpixel[2] = pr;
}

/*
 * Name: CompVtoRGB
 * Purpose: Convert the given Component video  pixel to RGB respresentation and 
 *          save the new components in the given RGB pixel.
 * Parameters: 
 *      float *CompVpixel : A pointer to a float array containing the pixels 
 *                          Y, pb, and pr values
 *          int *RGBpixel : A pointer to an integer array where the converted   
 *                          component video values will go.
 *        int denominator : The max val of the ppm image the component video 
 *                          pixels are being added to
 * Output: n/a
 * Effects: The given RGBpixel array is updated with the corresponding red, 
 *          green, and blue values.
 */
void CompVtoRGB(float *CompVpixel, int *RGBpixel, int denominator)
{
        /* get the component video values */
        float y = CompVpixel[0], pb = CompVpixel[1], pr = CompVpixel[2];
        
        /* compute the r g b vals */
        float red = (RED_Y * y) + (RED_PB * pb) + (RED_PR * pr);
        float green = (GREEN_Y * y) - (GREEN_PB * pb) - (GREEN_PR * pr);
        float blue = (1.0 * y) + (BLUE_PB * pb) + (BLUE_PR * pr);    
        
        /* scale them based on denomonator */
        red *= denominator;
        green *= denominator;
        blue *= denominator;

        /* make sure rgb vals stay inside range */
        red = fmin(denominator, fmax(RGB_LOWERBOUND, red));
        green = fmin(denominator, fmax(RGB_LOWERBOUND, green));
        blue = fmin(denominator, fmax(RGB_LOWERBOUND, blue));

        /* save the converted vals to the given RGB pixel */
        RGBpixel[0] = (int)red;
        RGBpixel[1] = (int)green;
        RGBpixel[2] = (int)blue;
}