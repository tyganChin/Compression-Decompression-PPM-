/*
 * Assignment: locality
 * Name: a2plain.c
 * Authors: Tygan Chin tchin02 & Benjamin Li bli14
 * Date: 10/10/23
 * Summary: This file implements the A2Methods interface for the UArray2 type.
 *          It contains static methods that call into UArray2 and provides
 *          an exported pointer to a struct so that the client uses the
 *          A2Methods function pointers instead of directly accessing the
 *          UArray2.
 */

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;   /* private abbreviation */

/*
 * new
 * purpose: makes a new UArray2 with specified dimensions and element size
 * parameters:
 *          int width: the desired width of the UArray2
 *         int height: the desired height of the UArray2
 *           int size: the number of bytes of each element of the UArray2
 * output: Returns the newly created UArray2
 * expectations: N/A
 */
static A2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/*
 * new_with_blocksize
 * purpose: makes a new UArray2 with specified dimensions and element size
 * parameters:
 *          int width: the desired width of the UArray2
 *         int height: the desired height of the UArray2
 *           int size: the number of bytes of each element of the UArray2
 *      int blocksize: the length of a block
 * output: Returns the newly created UArray2
 * expectations: N/A
 * notes: A plain UArray2 does not have specified blocks, so we ignore the given
 *        blocksize.
 */
static A2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/*
 * a2free
 * purpose: frees the memory associated with a UArray2
 * parameters:
 *          A2 *array2p: A pointer to the UArray2_T to be freed
 * output: none
 * expectations: N/A
 */
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *) array2p);
}

/*
 * width
 * purpose: gets the width of a UArray2
 * parameters:
 *          A2 array2: A pointer to a UArray2 to be accessed
 * output: Returns the width of the array
 * expectations: N/A
 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/*
 * height
 * purpose: gets the height of a UArray2
 * parameters:
 *          A2 array2: A pointer to a UArray2 to be accessed
 * output: Returns the height of the array
 * expectations: N/A
 */
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/*
 * size
 * purpose: gets the integer byte size of an element of a UArray2
 * parameters:
 *          A2 array2: A pointer to a UArray2 to be accessed
 * output: Returns the integer byte size of each element of a UArray2
 * expectations: N/A
 */
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/*
 * blocksize
 * purpose: gets the block size of a UArray2
 * parameters:
 *          A2 array2: A pointer to a UArray2 to be accessed
 * output: Always returns 1 as a plain UArray2 does not have specified blocks
 * expectations: N/A
 */
static int blocksize(A2 array2)
{
        (void) array2;
        return 1;
}

/*
 * at
 * purpose: gets the element at a specified location in a UArray2
 * parameters:
 *          A2 array2: A pointer to a UArray2 to be accessed
 *              int i: The desired column of the element
 *              int j: The desired row of the element
 * output: Returns a pointer to the element stored at the specified col/row of
 *         the UArray2
 * expectations: N/A
 */
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (applyfun *)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (applyfun *)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* a struct containing A2Methods function pointers, to be used by the client */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,
        map_col_major,
        NULL,                 /* map_block_major */
        map_row_major,        /* map_default */
        small_map_row_major,
        small_map_col_major,
        NULL,                 /* small_map_block_major */
        small_map_row_major,  /* small_map_default */
};

/* exported pointer to the struct */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;