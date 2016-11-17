#include <stdio.h>
#include <omp.h>
#include <math.h>

#define FILTER_SIZE 5
#define BLUR 0
#define SHARPEN 1
#define MOTION_BLUR 2
#define GAUSSIAN_BLUR 3

// Kernel Matrix
//int filter[7][7] = { { 0, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, -1, 0, 0, 0 },
//                     { 0, 0, -1, 5, -1, 0, 0 },
//                     { 0, 0, 0, -1, 0, 0, 0 },
//                     { 1, 0, 0, 0, 0, 0, 0 },
//                     { 0, 0, 0, 0, 0, 0, 0 } };

//blur
double filter[][FILTER_SIZE][FILTER_SIZE] = {
                        // Blur filter
                        {{ 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1}},

                        // Sharpen filter
                        {{ 0, 0, 0, 0, 0},
                         { 0, 0, -1, 0, 0},
                         { 0, -1, 5, -1, 0},
                         { 0, 0, -1, 0, 0},
                         { 0, 0, 0, 0, 0}},

                        // Motion Blur filter
                        {{ 1, 0, 0, 0, 0},
                         { 0, 1, 0, 0, 0},
                         { 0, 0, 1, 0, 0},
                         { 0, 0, 0, 1, 0},
                         { 0, 0, 0, 0, 1}},

                         // Gaussian filter
                        {{ 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1},
                         { 1, 1, 1, 1, 1}},



};

void generate_gaussian(double std){

    printf("%f", std);

    // set standard deviation to 1.0
    double r, s = 2.0 *std * std;

    // sum is for normalization
    double sum = 0.0;
    double start, end;



    start = omp_get_wtime();
    // generate 5x5 kernel
    for (int x = -FILTER_SIZE/2; x <= FILTER_SIZE/2; x++)
    {
        for(int y = -FILTER_SIZE/2; y <= FILTER_SIZE/2; y++)
        {
            r = sqrt(x*x + y*y);
            filter[GAUSSIAN_BLUR][x + FILTER_SIZE/2][y + FILTER_SIZE/2] = exp(-(r*r)/s)/(M_PI * s);
            printf("%f", filter[GAUSSIAN_BLUR][x + FILTER_SIZE/2][y + FILTER_SIZE/2] );
            sum += filter[GAUSSIAN_BLUR][x + FILTER_SIZE/2][y + FILTER_SIZE/2] ;
            printf("  sum = %f", sum);
        }
    }

    // normalize the Kernel
    for(int i = 0; i < FILTER_SIZE; ++i)
        for(int j = 0; j < FILTER_SIZE; ++j)
            filter[GAUSSIAN_BLUR][i][j] /= sum;

    end = omp_get_wtime();
    printf("\nTime taken for generating gaussian kernel sequentially : %f", (end-start));

//    for(int i=0; i< FILTER_SIZE; i++)
//    {
//        for(int j=0; j< FILTER_SIZE; j++)
//        printf("%f  ", filter[GAUSSIAN_BLUR][i][j]);
//        printf("\n");
//    }

    // sum is for normalization
    sum = 0.0;

    start = omp_get_wtime();
    // generate 5x5 kernel
#pragma omp parallel for collapse(2) num_threads(4) schedule(static, 8) default(shared) reduction(+:sum)
    for (int x = -FILTER_SIZE/2; x <= FILTER_SIZE/2; x++)
    {
        for(int y = -FILTER_SIZE/2; y <= FILTER_SIZE/2; y++)
        {
            r = sqrt(x*x + y*y);
            filter[GAUSSIAN_BLUR][x + FILTER_SIZE/2][y + FILTER_SIZE/2] = (exp(-(r*r)/s))/(M_PI * s);
            sum += filter[GAUSSIAN_BLUR][x + FILTER_SIZE/2][y + FILTER_SIZE/2] ;
        }
    }

    // normalize the Kernel
    for(int i = 0; i < FILTER_SIZE; ++i)
        for(int j = 0; j < FILTER_SIZE; ++j)
            filter[GAUSSIAN_BLUR][i][j] /= sum;

    end = omp_get_wtime();
    printf("\nTime taken for generating gaussian kernel in parallel : %f", (end-start));

}


int limit(int ); // Gives a boundary 0-255 for input value

int apply_filter(char *input_file, int filter_type)
{

    BMP* bmp;
    BMP* bmp2;

    UCHAR r, g, b;
    UINT width, height;
    UINT x,y;

    double rtot, btot, gtot;
    int norm_factor;
    int F_TYPE ;

    double start, end;
    double cpu_time_used;
    double std;


    F_TYPE = filter_type;

    bmp  = BMP_ReadFile(input_file);
    bmp2 = BMP_ReadFile(input_file);
    BMP_CHECK_ERROR(stderr, -1); // Error reading bmp file

    width  = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    printf("\n Width = %lu , Height = %lu", width, height);
    printf("\n Filter applied : %d", F_TYPE);

    if(filter_type == GAUSSIAN_BLUR)
    {
        printf("\nPlease enter STD: ");
        scanf("%lf", &std);
        generate_gaussian(std);
    }
    start = omp_get_wtime();

    #pragma omp parallel for default(shared) \
                             collapse(2)     \
                             num_threads(4)  \
                             schedule(static, 128) \
                             private(gtot,rtot,btot,norm_factor)
    for(int i=0; i<width; i++)
    {
        for(int j=0; j < height; j++)
        {
            rtot = gtot = btot = 0;
            norm_factor=FILTER_SIZE*FILTER_SIZE;
/*
            #pragma omp parallel for reduction(+:rtot), \
                                     reduction(+:btot), \
                                     reduction(+:gtot)  \
                                     collapse(2)        \
                                     default(shared)    \
                                     num_threads(2)\
                                     private(r,g,b) */
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

    end = omp_get_wtime();
    cpu_time_used = end-start;
    printf("\nThe time in performing this operation in parallel is %.2f", cpu_time_used);

    start = omp_get_wtime();

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

    end = omp_get_wtime();
    cpu_time_used = ((double) ( end - start ));
    printf("\nThe time in performing this operation sequentially is %.2f", cpu_time_used);

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