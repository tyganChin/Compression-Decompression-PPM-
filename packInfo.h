/*
 * Assignment: arith
 * Name: packInfo.h
 * Authors: Tygan Chin (tchin02)
 * Date: 10/19/23
 * Summary: Provides the width and locations of the quantized components in the 
 *          codeWord as well as two structs to hold quantized and dequantized 
 *          pixel values.
*/

#ifndef PACKINFO_H_INCLUDED
#define PACKINFO_H_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include "codeWord.h"

/* width and location of the packed data determined by size of the codeWord */
static const int WORD_BIT_LENGTH = sizeof(codeWord) * 8;
static const int PR_LSB = 0,                 PR_WIDTH = WORD_BIT_LENGTH / 8;
static const int PB_LSB = PR_WIDTH,          PB_WIDTH = WORD_BIT_LENGTH / 8;
static const int D_LSB  = PB_LSB + PB_WIDTH, D_WIDTH  = WORD_BIT_LENGTH / 6.40;
static const int C_LSB  = D_LSB + D_WIDTH,   C_WIDTH  = WORD_BIT_LENGTH / 6.40;
static const int B_LSB  = C_LSB + C_WIDTH,   B_WIDTH  = WORD_BIT_LENGTH / 6.40;
static const int A_LSB  = B_LSB + B_WIDTH,   A_WIDTH  = WORD_BIT_LENGTH - A_LSB;

/* struct to hold the quantized vals for the compression/decomp */
struct quantizedVals {
        uint64_t qA;
        int64_t qB, qC, qD;
        unsigned pbChroma, prChroma;
};

/* struct to hold the pixel values for compression/decomp */
struct pixelVals {
        float Y1, Y2, Y3, Y4;
        float pbAvg, prAvg;
};

#endif
