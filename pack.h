/*
 * Assignment: arith
 * Name: pack.h
 * Authors: Tygan Chin (tchin02)
 * Date: 10/18/23
 * Summary: Provides function declarations for a function that packs 4 given 
 *          pixels into a given codeWord and one that  unpacks a given codeWord 
 *          into 4 given pixels.
*/

#ifndef PACK_H_INCLUDED
#define PACK_H_INCLUDED

#include "codeWord.h"

void packWord(float **pix, codeWord *word);
void unpackWord(codeWord word, float **pix);

#endif
