#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
#include <omp.h>

unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight);
int* findImage(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight);
unsigned char * rgb_to_grey(unsigned char* input,int size);
unsigned char* optiGrey_to_greyImage(unsigned char *input,int width,int height);
int compareImage(unsigned char *img1,unsigned char *img2,int height, int width);
unsigned char * encadrerEnRouge(int x,int y,unsigned char* input,int inputWidth,int inputHeight,int searchWidth,int searchHeight);
int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid arguments !\n");
        return EXIT_FAILURE;
    }
    printf("lol\n");
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
    printf("Input image %s: %dx%d\n", inputImgPath, inputImgWidth, inputImgHeight);
    int nbValueRgb=inputImgWidth*inputImgHeight*3;
    int nbValueGrey=inputImgWidth*inputImgHeight;
    unsigned char *greyOpti = (unsigned char *)malloc(nbValueRgb  * sizeof(unsigned char));

   // unsigned char *greyImage = (unsigned char *)malloc(nbValueRgb  * sizeof(unsigned char));

   // greyOpti=rgb_to_grey(inputImg,nbValueGrey,nbValueRgb);
    //greyImage=optiGrey_to_greyImage(greyOpti,nbValueGrey,nbValueRgb);
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


    unsigned char *inputGrey = (unsigned char *)malloc(inputImgHeight*inputImgWidth*3  * sizeof(unsigned char));
    unsigned char *searchGrey = (unsigned char *)malloc(searchImgHeight*searchImgWidth*3  * sizeof(unsigned char));
    inputGrey=rgb_to_grey(inputImg,inputImgHeight*inputImgWidth*3);

    searchGrey=rgb_to_grey(searchImg,searchImgHeight*searchImgWidth*3);

    findImage(inputImg,inputImgWidth,inputImgHeight,searchImg,searchImgWidth,searchImgHeight);
    // ====================================  Save example: save a copy of 'inputImg'
     unsigned char *saveExample = (unsigned char *)malloc(inputImgWidth * inputImgHeight  * 3*sizeof(unsigned char));
    memcpy( saveExample, inputImg, inputImgWidth * inputImgHeight*3  * sizeof(unsigned char) );

    stbi_write_png("img/save_example.png", inputImgWidth, inputImgHeight, 3, saveExample, inputImgWidth * 3);

    free(saveExample);
    stbi_image_free(inputImg); 
    stbi_image_free(searchImg); 

    printf("Good bye!\n");

    return EXIT_SUCCESS;
}
unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight){
   unsigned char * petiteImage=(unsigned char *)malloc(searchImgWidth*searchImgHeight*3  * sizeof(unsigned char));
   int indexInPetiteImage=0;
   #pragma omp parallel for
   for(int i=x;i<x+searchImgHeight;i++){
       for(int j=y;j<y+searchImgWidth;j++){
           int index=(i*inputImgWidth+j)*3;
           petiteImage[indexInPetiteImage]=imgSource[index];
           petiteImage[indexInPetiteImage+1]=imgSource[index+1];
           petiteImage[indexInPetiteImage+2]=imgSource[index+2];
           indexInPetiteImage+=3;
       }
   }
    return petiteImage;
    
}
int compareImage(unsigned char *img1,unsigned char *img2,int height, int width){
    int difference =0;
    for(int i=0;i<height*width*3;i+=3){
        difference+=(img1[i]-img2[i])*(img1[i]-img2[i]);
    }
    return difference;
}
int*  findImage(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight){
    int indiceI=-1;
    int indiceJ=-1;
    int differenceMin=9999999999;
    #pragma omp parallel for
    for (int i=0;i<inputImgHeight;i++){
        printf("%d \n",i);
        for(int j=0;j<inputImgWidth;j++){
            if(inputImgHeight-i>=searchImgHeight && inputImgWidth-j>=searchImgWidth ){
                unsigned char * petiteImage = getPetiteImage(i,j,imgSource,inputImgWidth,inputImgHeight,searchImg,searchImgWidth,searchImgHeight);
                int differenceCourante=compareImage(petiteImage,searchImg,searchImgHeight,searchImgWidth);
                if(differenceCourante<differenceMin){
                                
                    differenceMin=differenceCourante;
                    indiceJ=j;
                    indiceI=i;
                } 
                free(petiteImage);
            }
                        

        }

    }
    if(indiceI!=-1){
        printf("%d %d \n",indiceI,indiceJ);
            encadrerEnRouge(indiceI,indiceJ,imgSource,inputImgWidth,inputImgHeight,searchImgWidth,searchImgHeight);

    }
}

unsigned char * rgb_to_grey(unsigned char* input_rgb,int size){
    unsigned char *grey_opti=(unsigned char *)malloc(size   * sizeof(unsigned char));
         int indexGrey=0;
         #pragma omp parallel for
    for(int i=0;i<size;i+=3){
        float value=0.299*input_rgb[i]+0.587*input_rgb[i+1]+0.114*input_rgb[i+2];
            grey_opti[indexGrey]=value;
            grey_opti[indexGrey+1]=value;
            grey_opti[indexGrey+2]=value;
            indexGrey+=3;
    }
    return grey_opti;
}
unsigned char * encadrerEnRouge(int x,int y,unsigned char* input,int inputWidth,int inputHeight,int searchWidth,int searchHeight){
     //trait du haut
     int index=0;
    for(int j=y;j<y+searchWidth;j++){
        index=(x*inputWidth+j)*3;
        input[index]=255;
        input[index+1]=0;
        input[index+2]=0;
     }
     //le bas
  for(int j=y;j<y+searchWidth;j++){
        index=((x+searchHeight)*inputWidth+j)*3;
        input[index]=255;
        input[index+1]=0;
        input[index+2]=0;
     }

     for(int j=x;j<x+searchHeight;j++){
        index=(j*inputWidth+y)*3;
        input[index]=255;
        input[index+1]=0;
        input[index+2]=0;
     }
     for(int j=x;j<x+searchHeight;j++){
        index=(j*inputWidth+y+searchWidth)*3;
        input[index]=255;
        input[index+1]=0;
        input[index+2]=0;
     }
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