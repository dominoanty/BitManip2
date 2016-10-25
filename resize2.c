#include <stdio.h>
#include <omp.h>


int li_resize_image( char *input_file)
{
    BMP* bmp;
    BMP* bmp2;

    UCHAR r, g, b;
    UCHAR r1, r2, r3, r4;
    UCHAR g1, g2, g3, g4;
    UCHAR b1, b2, b3, b4;
    UINT width, height;
    UINT new_width, new_height;
    UINT x,y;

    double x_ratio, y_ratio;
    double x_diff, y_diff;

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
            x_diff = (i * x_ratio) - (int)(i * x_ratio) ;
            y_diff = (j * y_ratio) - (int)(j * y_ratio) ;

            BMP_GetPixelRGB( bmp, (int)(i*x_ratio) , (int)(j*y_ratio), &r1, &g1, &b1);
            BMP_GetPixelRGB( bmp, (int)(i*x_ratio) + 1 , (int)(j*y_ratio), &r2, &g2, &b2);
            BMP_GetPixelRGB( bmp, (int)(i*x_ratio) , (int)(j*y_ratio) + 1, &r3, &g3, &b3);
            BMP_GetPixelRGB( bmp, (int)(i*x_ratio) + 1 , (int)(j*y_ratio) + 1, &r4, &g4, &b4);

            r = r1 * (1 - x_diff) * (1 - y_diff) + r2 * (x_diff) * (1 - y_diff) +
                r3 * (1 - x_diff) * (y_diff)     + r4 * (x_diff) * (y_diff);

            g = g1 * (1 - x_diff) * (1 - y_diff) + g2 * (x_diff) * (1 - y_diff) +
                g3 * (1 - x_diff) * (y_diff)     + g4 * (x_diff) * (y_diff);

            b = b1 * (1 - x_diff) * (1 - y_diff) + b2 * (x_diff) * (1 - y_diff) +
                 b3 * (1 - x_diff) * (y_diff)     + b4 * (x_diff) * (y_diff);

            BMP_SetPixelRGB( bmp2, i, j, r, g, b );
        }
    }

    BMP_WriteFile(bmp2, "output3.bmp");
    BMP_Free(bmp);
    BMP_Free(bmp2);
    return 0;

}