/*
 * Assignment: arith
 * Name: RGBcompvConvert.h
 * Authors: Tygan Chin (tchin02) & Emily Ye (eye03)
 * Date: 10/18/23
 * Summary: Provides user with the functions to convert a single 12 byte RGB 
 *          pixel to Componet video form and vice versa.
*/

#ifndef RGBCOMPVCONVERT_H_INCLUDED
#define RGBCOMPVCONVERT_H_INCLUDED

#include <stdio.h>

void RGBtoCompV(int *RGBpixel, float *CompVpixel, int denominator);
void CompVtoRGB(float *CompVpixel, int *RGBpixel, int denominator);

#endif