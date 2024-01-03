/*
 * Assignment: iii
 * Name: uarray2.h
 * Authors: David Chen zchen18 & Tygan Chin tchin02
 * Date: 9/20/2023
 * Summary: Interface for the uarray2 data structure. Allows user to create, 
 *          access, and change a 2d array of data.
*/

#ifndef UARRAY2_H_INCLUDED
#define UARRAY_H_INCLUDED

#include <stdio.h>
#include <uarray.h>

#define T UArray2_T 
typedef struct T *T;

T UArray2_new(int width, int height, int size);
void UArray2_free(T *UArray);
int UArray2_height(T UArray);
int UArray2_width(T UArray);
int UArray2_size(T UArray);
void *UArray2_at(T UArray, int col, int row);
void UArray2_map_row_major(T uarray2, 
                           void apply(int row, int col, T a, void *value, 
                           void*cl), 
                           void *cl);
void UArray2_map_col_major(T uarray2, 
                           void apply(int row, int col, T a, void *value, 
                           void*cl), 
                           void *cl);

#undef T
#endif