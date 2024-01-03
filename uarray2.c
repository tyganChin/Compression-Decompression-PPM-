/*
 * Assignment: iii
 * Name: uarray.c
 * Authors: David Chen zchen18 & Tygan Chin tchin02
 * Date: 9/20/2023
 * Summary: Implementation for the 2d array data strucutre (Uarray2_T). Allows 
 *          user to create, access, and modify an of data array using col and 
 *          row. 
 */

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray2.h"
#include <uarray.h>
#include <mem.h>

#define T UArray2_T

struct T {
        int height;
        int width;
        UArray_T Uarray;
};

/*
 * Name: Uarray2_new
 * Description: We create a 2d array of height * width on the heap with the 
 *              inputted byte size of each element. The slots of this uarray is 
 *              initialized to 0. 
 * Returns: A pointer to a Uarray2 object
 * Input: 
 *       int width : desired width of the uarray2 array
 *      int height : desired height of the uarray2 array 
 *        int size : desired size of each element in array
 * Expectations: Positive input values for height, width and size; CRE when 
 *               memory allocation fails and the dimensions are negative.
 * Note : User is responsible to free the allocated Uarray2.
 */
T UArray2_new(int width, int height, int size) 
{
        /* allocate the array */
        T UArray2;
        NEW(UArray2);

        /* initialize it contents */
        UArray2->Uarray = UArray_new(height * width, size);
        UArray2->width = width;
        UArray2->height = height;

        return UArray2;
}

/*
 * Name: UArray_free
 * Description: Frees the memory associated with the UArray by freeing each 
 *              array individually
 * Input: 
 *      T *uarray2 : The pointer of Uarray 2 whose memory is to be freed
 * Returns: n/a
 * Expectations: CRE when array pointer is NULL or uarray itslef is NULL.
 */
void UArray2_free(T *uarray2) 
{
        assert(*uarray2 != NULL);
        assert((*uarray2)->Uarray != NULL);

        UArray_free(&(*uarray2)->Uarray);
        FREE(*uarray2);
}

/*
 * Name: Urray2_height
 * Description: Accesses the height value stored in the UArray struct and 
 *              returns it
 * Input:   
 *      T uarray2 : A pointer to a uarray2
 * Returns: The integer height of the Uarray
 * Expectations: CRE when array pointer is NULL
 */
int UArray2_height(T uarray2) 
{
        assert(uarray2 != NULL);
        return uarray2->height;
}

/*
 * Name: UArray_width
 * Description: Accesses the width value stored in the UArray struct and 
 *              returns it
 * Input:   
 *      T uarray2 : A pointer to a uarray2
 * Returns: The integer width of the Uarray
 * Expectations: CRE when array pointer is NULL
 */
int UArray2_width(T uarray2) 
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

/*
 * Name: UArray_size
 * Description: Accesses the size value stored in the UArray struct and returns 
 *              it
 * Input:   
 *      T uarray2 : A pointer to a uarray2
 * Returns: The integer byte size of a single element in the Uarray
 * Expectations: CRE when array pointer is NULL
 */
int UArray2_size(T uarray2) 
{
        assert(uarray2 != NULL);
        return UArray_size(uarray2->Uarray);
}

/*
 * Name: UArray_at
 * Description: Accesses the value stored in the given UArray at the given row 
 *              and col
 * Input: 
 *      T uarray2 : A pointer to a uarray2
 *        int col : integer col of the value,
 *        int row : integer row of the value
 * Returns: A void pointer to the value at the given row and col
 * Expectations: Row and col are within the boundaries of the UArray (height < 
 *               row >= 0 and width < col >= 0); the given array pointer is not 
 *               null; call CRE if the above requirements fails
 */
void *UArray2_at(T uarray2, int col, int row) 
{
        assert(uarray2 != NULL);
        assert(row < uarray2->height && row >= 0);
        assert(col < uarray2->width && col >= 0);
        return UArray_at(uarray2->Uarray, col + row * uarray2->width);
}

/*
 * Name: UArray_map_row_major
 * Description: Iterates through the UArray and applies the given function row 
 *              by row.
 * Input: 
 *      T uarray2 : A pointer to a uarray2
 *      void apply : Function to apply to every space in the uarray
 *              int col :  integer col of the value
 *              int row :  integer row of the value
 *              T a : A pointer to a uarray2
 *              void *value : A void pointer to the value stored col,row of a
 *              void *cl : A pointer to value used to store information between 
 *                        iterations
 *      void *cl : A pointer to value used to store information between 
 *                 iterations
 * Returns: n/a
 * Effects: The given “cl” void pointer is updated in the function where the 
 *          map function is called.
 * Expectations: CRE when array pointer is NULL
 */
void UArray2_map_row_major(T uarray2, void apply(int col, int row, T a,
                           void *value, void *cl), void *cl) 
{
        assert(uarray2 != NULL);
        assert(apply != NULL);
        for (int row_i = 0; row_i < uarray2->height; ++row_i) {
                for (int col_i = 0; col_i < uarray2->width; ++col_i) {
                        apply(col_i, row_i, uarray2, UArray2_at(uarray2, col_i, row_i), cl);
                }
        }
}

/*
 * Name: UArray_map_col_major
 * Description: Iterates through the UArray and applies the given function col by col.
 * Input: 
 *      T uarray2 : A pointer to a uarray2
 *      void apply : Function to apply to every space in the uarray
 *              int col :  integer col of the value
 *              int row :  integer row of the value
 *              T a : A pointer to a uarray2
 *              void *value : A void pointer to the value stored col,row of a
 *              void *cl : A pointer to value used to store information between 
 *                         iterations
 *      void *cl : A pointer to value used to store information between 
 *                 iterations
 * Returns: n/a
 * Effects: The given “cl” void pointer is updated in the function where the 
 *          map function is called.
 * Expectations: CRE when array pointer is NULL
 */
void UArray2_map_col_major(T uarray2, void apply(int col, int row, T a, 
                           void *value, void*cl), void *cl) 
{
        assert(uarray2 != NULL);
        assert(apply != NULL);
        for (int col_i = 0; col_i < uarray2->width; ++col_i) {
                for (int row_i = 0; row_i < uarray2->height; ++row_i) {
                        apply(col_i, row_i, uarray2, UArray2_at(uarray2, col_i, row_i), cl);
                }
        }
}
