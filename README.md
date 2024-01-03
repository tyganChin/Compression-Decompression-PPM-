# Compression-Decompression-PPM-
#### Main Author : Tygan Chin
- ##### Sub-Authors : Tufts CS Adim, Emily Ye, Benjamin Li, David Chen
#### Language : C
#### Date : October 16, 2023



## Description
Takes in a PPM image/compressed PPM image and compresses/decompresses it into a text file about 1/4 the size of the original file or restores the original image. The compression is lossy and is accomplished through color space conversion, bitpacking, and quantization.

## Architecture

- **40image.c** - Runs compression program

    - **compress40** -Compresses/decompresses the given image 
  
        - **codeWord.h**  -  Defines the codeWord type

        - **RGBcompvConvert.c** - Converts a given pixel from RGB/CompV to CompV/RGB video
                                  space
          
        - **pack.c**  - Packs the given pixels into a single word or unpacks word into pixels

            - **bitpack.c** - Gives ability to pack up to a 64-bit integer
              
            - **packInfo.h** - Defines the structs and component locations within the
                               codeWord   
         
            - **quantize.c** - Quantizes/dequantizes the given pixels  
