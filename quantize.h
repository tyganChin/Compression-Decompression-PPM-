/*
 * Assignment: arith
 * Name: quantize.h
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/20/23
 * Summary: Provides a quantization and dequantization function which quantiz 
*/

#ifndef QUANTIZE_H_INCLUDED
#define QUANTIZE_H_INCLUDED

#include <stdio.h>
#include "packInfo.h"

void quantize(struct pixelVals *pVals, struct quantizedVals *qVals);
void dequantize(struct quantizedVals *qVals, struct pixelVals *pVals);

#endif