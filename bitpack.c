/*
 * Assignment: arith
 * Name: bitpack.c
 * Authors: Tygan Chin (tchin02)
 * Date: 10/18/23
 * Summary: Implementation of the bitpack interface. Allows user to create a 
 *          signed or unsigned 64 bitpacked integer. Functions allow user to 
 *          add new sequences as well as access prexisting ones in within the 
 *          given packed word.
 */

#include <stdio.h>
#include <stdlib.h>
#include "bitpack.h"
#include <assert.h>
#include <math.h>
#include "except.h"

/* helper functions */
static uint64_t lshift(uint64_t value, int n);
static uint64_t rshift_u(uint64_t value, int n);
static int64_t rshift_s(int64_t value, int n);

/* exception to throw if bit width is outside of the 64 bit word */
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Name: Bitpack_fitsu
 * Purpose: Determines whether or not the given unsigned integer fits in 
 *          "width" spaces in binary.
 * Parameters: 
 *           int64_t n : The integer that is to be checked
 *      unsigned width : The width (in the bits)
 * Returns: True if the integer fits in the width, false if not.
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        uint64_t upperBound = lshift(1, width) - 1;
        return (n <= upperBound);
}

/*
 * Name: Bitpack_fitss
 * Purpose: Determines whether or not the given signed integer fits in 
 *          "width" spaces in binary.
 * Parameters: 
 *           int64_t n : The integer that is to be checked
 *      unsigned width : The width (in bits)
 * Returns: True if the integer fits in the width, false if not.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        int64_t lowerBound = ~lshift(1, width - 1) + 1;
        int64_t upperBound = lshift(1, width - 1) - 1;
        return (n >= lowerBound && n <= upperBound); 
}

/*
 * Name: Bitpack_getu
 * Purpose: Get the unsigned integer stored at the given width and position 
 *          (lsb) in the given word.
 * Parameters: 
 *       uint64_t word : A 64 bit unsigned integer containing the stored 
 *                       information.
 *      unsigned width : The width (in bits) of the integer to retrieve
 *        unsigned lsb : The least significant bit of the integer in the word 
 *                       to retrieve (location)
 * Returns: The integer stored at the given location as an unsigned 64 bit 
 *          integer.
 * Expectations: The width is less than or equal to the size of the word (64) 
 *               and the start of the word plus the width exist within the size 
 *               of the word. CRE if this is violated. 
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        /* ensure inputs are valid for a 64 bit word */
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* create mask with "width" consecutive 1s at the desired data seq */
        uint64_t mask = lshift((lshift(1, width) - 1), lsb);

        /* copy the data in specified data seq in word to the mask */
        mask &= word;

        /* shift mask back to begining at 0 */
        mask = rshift_u(mask, lsb);

        return mask;
}

/*
 * Name: Bitpack_getu
 * Purpose: Get the signed integer stored at the given width and position (lsb) 
 *          in the given word.
 * Parameters: 
 *        int64_t word : A 64 bit signed integer containing the stored 
 *                       information.
 *      unsigned width : The width (in bits) of the integer to retrieve
 *        unsigned lsb : The least significant bit of the integer in the word 
 *                       to retrieve (location)
 * Returns: The integer stored at the given location as an signed 64 bit 
 *          integer.
 * Expectations: The width is less than or equal to the size of the word (64) 
 *               and the start of the word plus the width exist within the size 
 *               of the word. CRE if this is violated. 
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        /* get the sequence within the word */
        int64_t mask = Bitpack_getu((int64_t)word, width, lsb);

        /* sign extend if most significant bit is 1 (negative) */
        if (rshift_s(mask, width - 1) == 1) {

                /* create a 64 bit int with all 1s */
                uint64_t extender = ~0;

                /* move extender to end of the number stored in the mask */
                extender = lshift(extender, width);

                /* add the ones to the 0s prior to the stored number (negate) */
                mask |= extender;

        }

        /* return the sequence */
        return mask;
}

/*
 * Name: Bitpack_news
 * Purpose: Put the given unsigned integer of size "width" at the given 
 *          location (lsb) of the given word and return the new word.
 * Parameters: 
 *        uint64_t word : A 64 bit signed integer 
 *      unsigned width : The width (in bits) of the integer to retrieve
 *        unsigned lsb : The least significant bit of the integer in the word 
 *                       to retrieve (location)
 *       uint64_t value : The unsigned integer to put into the given word
 * Returns: A 64 bit unsigned integer containing the given value
 * Expectations: The width is less than or equal to the size of the word (64) 
 *               and the start of the word plus the width exist within the size 
 *               of the word. Also, the given value should be able to fit 
 *               within the designated width. CRE if any of these are violated
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        /* make sure width and first sig byte are valid */
        assert(width <= 64);
        assert(width + lsb <= 64);

        /* check if value fits in the width */
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* move value to correct place in word */
        value = lshift(value, lsb);
     
        /* create mask of 1s left of the segment */
        uint64_t mask_left = ~0;
        mask_left = lshift(mask_left, width + lsb);

        /* create mask of 1s right of the segment */
        uint64_t mask_right = lshift(1, lsb) - 1;

        /* join masks with a mask of all 0s */
        uint64_t mask = mask_left | mask_right;
        
        /* clear the old segmenet stored in the word */
        word &= mask;

        /* add the new segement */
        word |= value;

        return word;
}

/*
 * Name: Bitpack_news
 * Purpose: Put the given signed integer of size "width" at the given location 
 *          (lsb) of the given word and return the new word.
 * Parameters: 
 *        uint64_t word : A 64 bit unsigned integer 
 *      unsigned width : The width (in bits) of the integer to retrieve
 *        unsigned lsb : The least significant bit of the integer in the word 
 *                       to retrieve (location)
 *       int64_t value : The signed integer to put into the given word
 * Returns: A 64 bit unsigned integer containing the given value
 * Expectations: The width is less than or equal to the size of the word (64) 
 *               and the start of the word plus the width exist within the size 
 *               of the word. Also, the given value should be able to fit 
 *               within the designated width. CRE if any of these are violated
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,  
                      int64_t value)
{
        /* check if the value fits in the width */
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* convert value to unsigned representation */
        uint64_t newValue = lshift(1, width) - 1;
        newValue &= value;

        return Bitpack_newu(word, width, lsb, newValue);
}


/*******************************************************************************
*                            Helper Functions                                  *
*******************************************************************************/

/*
 * Name: lshift
 * Purpose: Shifts the given unsigned 64 bit integer value by n.
 * Parameters: 
 *      uint64_t value : The value to be shifted
 *               int n : The number places to be shifted left
 * Output: The left-shifted integer. Returns 0 if shift is greater than or 
 *         equal to 64
 */
uint64_t lshift(uint64_t value, int n) 
{
        if (n >= 64) {
                return 0; 
        } else {
                return value << n;
        }
}

/*
 * Name: rshift_u
 * Purpose: Shifts the given unsigned 64 bit integer value by n.
 * Parameters: 
 *      uint64_t value : The value to be shifted
 *               int n : The number places to be shifted left
 * Output: The right-shifted integer. Returns 0 if shift is greater than or 
 *         equal to 64
 */
uint64_t rshift_u(uint64_t value, int n) 
{
        if (n >= 64) {
                return 0;
        } else {
                return value >> n;
        }
}

/*
 * Name: rshift_s
 * Purpose: Shifts the given signed 64 bit integer value by n.
 * Parameters: 
 *      uint64_t value : The value to be shifted
 *               int n : The number places to be shifted left
 * Output: The right-shifted integer. If shift is greater than or equal to 64, 
 *         returns 0 if num is positve and -1 if num is negative.
 */
int64_t rshift_s(int64_t value, int n) 
{
        if (n >= 64 && value >= 0) {
                return 0;
        } else if (n >= 64 && value < 0) {
                return -1;
        } else {
                return value >> n;
        }
}
