#include <stdio.h>
#include <omp.h>

#define FILTER_SIZE 3
#define BLUR 0
#define SHARPEN 1
#define MOTION_BLUR 2

// Kernel Matrix
//int filter[7][7] = { { 0, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, -1, 0, 0, 0 },
//                     { 0, 0, -1, 5, -1, 0, 0 },
//                     { 0, 0, 0, -1, 0, 0, 0 },
//                     { 1, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, 0, 0, 0, 0 } };

//blur
double filter[][3][3] = {
                        // Blur filter
                        {{ 1, 1, 1},
                         { 1, 1, 1},
                         { 1, 1, 1}},

                        // Sharpen filter
                        {{ 0, -0.5, 0},
                         { -0.5, 3, -0.5},
                         { 0, -0.5, 0}},

                        // Motion Blur filter
                        {{ 1, 0, 0},
                         { 0, 1, 0},
                         { 0, 0, 1}}

};

int limit(int ); // Gives a boundary 0-255 for input value

int apply_filter(char *given_input_file, int filter_type)
{

    char input_file[20];
    strcpy(input_file, given_input_file);
    BMP* bmp;
    BMP* bmp2;

    UCHAR r, g, b;
    UINT width, height;
    UINT x,y;

    int rtot, btot, gtot;
    int norm_factor;
    int F_TYPE ;


    F_TYPE = filter_type;

    bmp  = BMP_ReadFile(input_file);
    bmp2 = BMP_ReadFile(input_file);
    BMP_CHECK_ERROR(stderr, -1); // Error reading bmp file

    width  = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    printf("\n Width = %lu , Height = %lu", width, height);
    printf("\n Filter applied : %d", F_TYPE);


    for(int i=0; i<width; i++)
    {
        for(int j=0; j < height; j++)
        {
            rtot = gtot = btot = 0;
            norm_factor=FILTER_SIZE*FILTER_SIZE;
            for(int k=-(FILTER_SIZE/2); k<=(FILTER_SIZE/2); k++)
            {
                for(int l=-(FILTER_SIZE/2);l<=(FILTER_SIZE/2); l++)
                {
                    //Make sure the filter doesn't apply out of the window size
                    if( ((i+k) < 0)      || ((j+l) < 0)      ||
                        ((i+k) >= width) || ((j+l) >= height)||
                        (filter[F_TYPE][k+(FILTER_SIZE/2)][l+(FILTER_SIZE/2)] == 0))
                    {
                        norm_factor--;
                    }
                    else
                    {
                        BMP_GetPixelRGB(bmp, (i+k) , (j+l) , &r , &g, &b );
                        rtot += r * filter[F_TYPE][k+(FILTER_SIZE/2)][l+(FILTER_SIZE/2)];
                        gtot += g * filter[F_TYPE][k+(FILTER_SIZE/2)][l+(FILTER_SIZE/2)];
                        btot += b * filter[F_TYPE][k+(FILTER_SIZE/2)][l+(FILTER_SIZE/2)];
                    }
                }
            }
            if(F_TYPE == BLUR || F_TYPE == MOTION_BLUR)
            {
                if(norm_factor == 0)
                {
                   // printf("\ni = %d, j=%d,:w", i,j);
                    BMP_SetPixelRGB(bmp2, i, j, 0, 0, 0);
                }
                else
                {
                    BMP_SetPixelRGB(bmp2, i, j, rtot/norm_factor, gtot/norm_factor, btot/norm_factor);
                }
            }
            else
            {

                BMP_SetPixelRGB(bmp2, i, j, limit(rtot), limit(gtot), limit(btot));
            }

        }
    }
    BMP_WriteFile(bmp2, "output.bmp");
    BMP_CHECK_ERROR(stderr, -2);

    BMP_Free(bmp);
    BMP_Free(bmp2);
    return 0;

}

int limit(int n)
{
    if(n > 255)
        return 255;
    else if(n < 0)
        return 0;
    else
        return n;
}