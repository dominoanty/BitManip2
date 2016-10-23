#include <stdio.h>
#include <math.h>
#include <omp.h>


int resize_image( char *input_file)
{
    BMP* bmp;
    BMP* bmp2;

    UCHAR r, g, b;
    UINT width, height;
    UINT new_width, new_height;
    UINT x,y;

    double x_ratio, y_ratio;

    printf("\nInput file %s" , input_file);
    bmp  = BMP_ReadFile(input_file);
    BMP_CHECK_ERROR(stderr, -1); // Error reading bmp file

    width  = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    printf("\n Width = %lu, Height = %lu", width, height);

    printf("\nEnter the width and the height after resizing  ");
    scanf("%lu", &new_width);
    scanf("%lu", &new_height);

    x_ratio = width  / (double) new_width;
    y_ratio = height / (double) new_height;

    bmp2 = BMP_Create(new_width, new_height, BMP_GetDepth(bmp));

    for(UINT i=0; i<new_width ; i++)
    {
        for(UINT j=0; j<new_height; j++)
        {
            BMP_GetPixelRGB( bmp, (int)(i*x_ratio) , (int)(j*y_ratio), &r, &g, &b);
            BMP_SetPixelRGB( bmp2, i, j, r, g, b );
        }
    }

    BMP_WriteFile(bmp2, "output2.bmp");
    BMP_Free(bmp);
    BMP_Free(bmp2);
    return 0;

}