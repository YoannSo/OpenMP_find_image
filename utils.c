#include "utils.h"

#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"

unsigned char* getGrey(const  char* path,int* width,int* height)
{
 
    int inputImgWidth;
    int inputImgHeight;
    int dummyNbChannels; // number of channels forced to 3 in stb_load.
    unsigned char *inputImg = stbi_load(path, &inputImgWidth, &inputImgHeight, &dummyNbChannels, 3);
    if (inputImg == NULL)
    {
        printf("Cannot load image %s", path);
        return EXIT_FAILURE;
    }
    printf("Input image %s: %dx%d\n", path, inputImgWidth, inputImgHeight);
        printf("lol");

    unsigned char* inGrey = (unsigned char *)malloc(inputImgHeight*inputImgWidth * sizeof(unsigned char));

    inGrey=rgb_to_grey(inputImg,inputImgHeight*inputImgWidth);
    *width=inputImgWidth;
    *height=inputImgHeight;
    return inGrey;
}
unsigned char* getNormal(const  char* path,int* width,int* height)
{
 
    int inputImgWidth;
    int inputImgHeight;
    int dummyNbChannels; // number of channels forced to 3 in stb_load.
    unsigned char *inputImg = stbi_load(path, &inputImgWidth, &inputImgHeight, &dummyNbChannels, 3);
    if (inputImg == NULL)
    {
        printf("Cannot load image %s", path);
        return EXIT_FAILURE;
    }
    printf("Input image %s: %dx%d\n", path, inputImgWidth, inputImgHeight);
        printf("lol");


    *width=inputImgWidth;
    *height=inputImgHeight;
    return inputImg;
}
void save(unsigned char* saveImg,int width,int height){
    unsigned char *saveExample = (unsigned char *)malloc(width * height  * 3*sizeof(unsigned char));
    memcpy( saveExample, saveImg, width * height*3  * sizeof(unsigned char) );

    stbi_write_png("img/save_example.png", width, height, 3, saveExample, width * 3);

    free(saveExample);
    stbi_image_free(saveImg); 
}
unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight){
   unsigned char * petiteImage=(unsigned char *)malloc(searchImgWidth*searchImgHeight  * sizeof(unsigned char));
   int indexInPetiteImage=0;
   for(int i=x;i<x+searchImgHeight;i++){
       for(int j=y;j<y+searchImgWidth;j++){
           int index=i*inputImgWidth+j;
           petiteImage[indexInPetiteImage]=imgSource[index];
           indexInPetiteImage++;
       }
   }
    return petiteImage;
    
}
int compareImage(unsigned char *img1,unsigned char *img2,int height, int width){
    int difference =0;
    for(int i=0;i<height*width;i++){
        difference+=(img1[i]-img2[i])*(img1[i]-img2[i]);
    }
    return difference;
}

int*  test(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int*  result){
    int indiceI=-1;
    int indiceJ=-1;
    int differenceMin=9999999999;
    for (int i=0;i<inputImgHeight;i++){
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
        result[0]=differenceMin;
        result[1]=indiceI;
        result[2]=indiceJ;
        }

}
unsigned char * rgb_to_grey(unsigned char* input_rgb,int size){
    unsigned char *grey_opti=(unsigned char *)malloc(size   * sizeof(unsigned char));
         int indexGrey=0;
    for(int i=0;i<size*3;i+=3){
        float value=0.299*input_rgb[i]+0.587*input_rgb[i+1]+0.114*input_rgb[i+2];
            grey_opti[indexGrey]=value;
            indexGrey++;
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
int*  findImageInImage(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* resultI,int* resultJ){
    int indiceI=-1;
    int indiceJ=-1;
    int differenceMin=9999999999;
    int indices [2];
    for (int i=0;i<inputImgHeight;i++){
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
        *resultI=indiceI;
        *resultJ=indiceJ;
        printf("la dif %d",differenceMin);
    }
}