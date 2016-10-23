#include <stdio.h>
#include <string.h>
#include "libs/qdbmp.h"
#include "libs/qdbmp.c"
#include "filter.c"
#include "resize.c"

int main(int argc, char *argv[])
{
    char input_file[20];
    int  filter_type;
    int  resize_ratio;
    int  option;

    if(argc !=2 )
    {
        fprintf(stderr, "Usage: %s <input file> ", argv[0]);
    }

    strcpy(input_file, argv[1]);
    BMP* bmp= BMP_ReadFile(input_file);
    do
    {
        printf("\nEnter your option :");
        printf("\n1. Apply Filter");
        printf("\n2. Resize (Nearest Neighbor)");
        printf("\n3. Exit");
        printf("\nEnter your option: ");
        scanf("%d", &option);
        switch(option)
        {
            case 1:
                do
                {
                    printf("\n What filter : ");
                    printf("\n 1. Blur ");
                    printf("\n 2. Sharpen " );
                    printf("\n 3. Motion Blur");
                    scanf("%d", &filter_type);
                }while(filter_type < 1 && filter_type > 3);
                apply_filter(input_file, --filter_type);
                break;

            case 2:
                if(resize_image(input_file) != -1)
                {
                    printf("\nImage resized successfully");
                }
                else
                {
                    fprintf(stderr, "Error occurred while resizing");
                };
                break;

            default:
                printf("\n Incorrect option");
        }
    }while(option != 3);


    return 0;
}
