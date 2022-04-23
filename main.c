#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeigh);
void findImage(int* x,int* y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeigh);
unsigned char * rgb_to_grey(unsigned char* input,int width,int height);
unsigned char* optiGrey_to_greyImage(unsigned char *input,int width,int height);


int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid arguments !\n");
        return EXIT_FAILURE;
    }

    // Get image paths from arguments.
    const char *inputImgPath = argv[1];
    const char *searchImgPath = argv[2];
/*f (argc != 3)
    {
        printf("Invalid arguments !\n");
        return EXIT_FAILURE;
    }

    // Get image paths from arguments.
    const char *inputImgPath = argv[1];
    const char *searchImgPath = argv[2];

    // ==================================== Loading input image.
    int inputImgWidth;
    int inputImgHeight;
    int dummyNbChannels; // number of channels forced to 3 in stb_load.
    unsigned char *inputImg = stbi_load(inputImgPath, &inputImgWidth, &inputImgHeight, &dummyNbChannels, 3);
    if (inputImg == NULL)
    {
        printf("Cannot load image %s", inputImgPath);
        return EXIT_FAILURE;
    }
    // faire chez moi
    printf("Input image %s: %dx%d\n", inputImgPath, inputImgWidth, inputImgHeight);
    unsigned char *greyImage = (unsigned char *)malloc(inputImgWidth * inputImgHeight  * sizeof(unsigned char));
    greyImage=rgb_to_grey(inputImg,inputImgWidth,inputImgHeight);
    // ====================================  Loading search image.
    int searchImgWidth;
    int searchImgHeight;
    unsigned char *searchImg = stbi_load(searchImgPath, &searchImgWidth, &searchImgHeight, &dummyNbChannels, 3);
    if (searchImg == NULL)
    {
        printf("Cannot load image %s", searchImgPath);
        return EXIT_FAILURE;
    }
    printf("Search image %s: %dx%d\n", searchImgPath, searchImgWidth, searchImgHeight);


    // ====================================  Save example: save a copy of 'inputImg'
    unsigned char *saveExample = (unsigned char *)malloc(inputImgWidth * inputImgHeight  * sizeof(unsigned char));
    memcpy( saveExample, greyImage, inputImgWidth * inputImgHeight  * sizeof(unsigned char) );

    stbi_write_png("img/save_example.png", inputImgWidth, inputImgHeight, 3, saveExample, inputImgWidth * 3);

    free(saveExample);
    stbi_image_free(inputImg); 
    stbi_image_free(searchImg); 

    printf("Good bye!\n");

    return EXIT_SUCCESS;*/

    
    // ==================================== Loading input image.
    int inputImgWidth;
    int inputImgHeight;
    
    int dummyNbChannels; // number of channels forced to 3 in stb_load.
    unsigned char *inputImg = stbi_load(inputImgPath, &inputImgWidth, &inputImgHeight, &dummyNbChannels, 3);
    if (inputImg == NULL)
    {
        printf("Cannot load image %s", inputImgPath);
        return EXIT_FAILURE;
    }
    printf("Input image %s: %dx%d\n", inputImgPath, inputImgWidth, inputImgHeight);
    int nbValueRgb=inputImgWidth*inputImgHeight*3;
    int nbValueGrey=inputImgWidth*inputImgHeight;
    unsigned char *greyOpti = (unsigned char *)malloc(nbValueGrey  * sizeof(unsigned char));
    unsigned char *greyImage = (unsigned char *)malloc(nbValueRgb  * sizeof(unsigned char));

    greyOpti=rgb_to_grey(inputImg,nbValueGrey,nbValueRgb);
    greyImage=optiGrey_to_greyImage(greyOpti,nbValueGrey,nbValueRgb);
    // ====================================  Loading search image.
    int searchImgWidth;
    int searchImgHeight;
    unsigned char *searchImg = stbi_load(searchImgPath, &searchImgWidth, &searchImgHeight, &dummyNbChannels, 3);
    if (searchImg == NULL)
    {
        printf("Cannot load image %s", searchImgPath);
        return EXIT_FAILURE;
    }
    printf("Search image %s: %dx%d\n", searchImgPath, searchImgWidth, searchImgHeight);



    // ====================================  Save example: save a copy of 'inputImg'
     unsigned char *saveExample = (unsigned char *)malloc(inputImgWidth * inputImgHeight  * 3*sizeof(unsigned char));
    memcpy( saveExample, greyImage, inputImgWidth * inputImgHeight*3  * sizeof(unsigned char) );

    stbi_write_png("img/save_example.png", inputImgWidth, inputImgHeight, 3, saveExample, inputImgWidth * 3);

    free(saveExample);
    stbi_image_free(inputImg); 
    stbi_image_free(searchImg); 

    printf("Good bye!\n");

    return EXIT_SUCCESS;
}
unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeigh){
    if (x+searchImgWidth>inputImgWidth){
        return NULL;    
    } else if (y+searchImgHeigh>inputImgHeight)
    {
        return NULL;
    } else {
        // Malloc de l'image
        unsigned char * newImage;
        for(int j=y;j<y+searchImgHeigh;j++){
            for(int i=x;i<x+searchImgHeigh;i++){
                printf("ca marche pas");
            }
        }
    }
    
}

void findImage(int* x,int* y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeigh){
    for (int j=0;j<inputImgHeight;j++){
        for (int i=0;i<inputImgWidth;i++){
            unsigned char * petiteImage = getPetiteImage(i,j,imgSource,inputImgWidth,inputImgHeight,searchImg,searchImgWidth,searchImgHeigh);
            
        }
    }
}

unsigned char * rgb_to_grey(unsigned char* input_rgb,int nbValueGrey,int nbValueRGB){
    unsigned char *grey_opti=(unsigned char *)malloc(nbValueGrey   * sizeof(unsigned char));
         int indexGrey=0;
    for(int i=0;i<nbValueRGB;i+=3){
            grey_opti[indexGrey]=0.299*input_rgb[i]+0,587*input_rgb[i+1]+0.114*input_rgb[i+2];
            indexGrey++;
    }
    return grey_opti;
}

unsigned char* optiGrey_to_greyImage(unsigned char *input_grey,int nbValueGrey,int nbValueRGB){
     unsigned char *grey_img=(unsigned char *)malloc(nbValueRGB* sizeof(unsigned char));
     int j=0;
     for(int i=0;i<nbValueGrey;i++){
        grey_img[j]=input_grey[i];
        grey_img[j+1]=input_grey[i];
        grey_img[j+2]=input_grey[i];
        j+=3;
    }
    return grey_img;
}