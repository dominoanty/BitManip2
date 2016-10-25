# BitManip2
Comparing performance and overheads of image processing algorithms applied in serial and in parallel to bitmap files in C. For this, [QNDBMP](http://qdbmp.sourceforge.net/) is being used to read in and write out the images. Also, the BMP data structure provided by this library is used.

## resize.c
Implements nearest neighbors image scaling. 

## resize2.c 
Implements bilinear interpolation image scaling. 

## filters.c 
Apply filters such as sharpen, blur filters on images. 

