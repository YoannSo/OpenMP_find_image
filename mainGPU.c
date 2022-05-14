#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
#include <omp.h>

#define NBCOEURS 4

unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight);
int* findImage(int debutW,int debutH,int moitieW,int moitieH,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* resultI,int* resultJ);
int*  test(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* result);
unsigned char * rgb_to_grey(unsigned char* input,int size);
unsigned char* optiGrey_to_greyImage(unsigned char *input,int width,int height);
int compareImage(unsigned char *img1,unsigned char *img2,int height, int width);
unsigned char * encadrerEnRouge(int x,int y,unsigned char* input,int inputWidth,int inputHeight,int searchWidth,int searchHeight);
int* finalresult;
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

    int tailleATraiter=(inputImgWidth-searchImgWidth)*(inputImgHeight*searchImgHeight);
    int tranche=tailleATraiter;
    int moitieH=inputImgHeight*0.5;
    int moitieW=inputImgWidth*0.5;

    unsigned char *inputGrey = (unsigned char *)malloc(inputImgHeight*inputImgWidth * sizeof(unsigned char));
    unsigned char *searchGrey = (unsigned char *)malloc(searchImgHeight*searchImgWidth  * sizeof(unsigned char));
    inputGrey=rgb_to_grey(inputImg,inputImgHeight*inputImgWidth);

    searchGrey=rgb_to_grey(searchImg,searchImgHeight*searchImgWidth);
    unsigned char *hg = (unsigned char *)malloc((moitieW+searchImgWidth)*(moitieH+searchImgHeight)  * sizeof(unsigned char));
    unsigned char *hd = (unsigned char *)malloc((moitieW)*(moitieH+searchImgHeight)  * sizeof(unsigned char));
    unsigned char *bg = (unsigned char *)malloc((moitieW+searchImgWidth)*(moitieH)  * sizeof(unsigned char));
    unsigned char *bd = (unsigned char *)malloc((moitieW)*(moitieH)  * sizeof(unsigned char));

    hg=getPetiteImage(0,0,inputGrey,inputImgWidth,inputImgHeight,searchGrey,moitieW+searchImgWidth,moitieH+searchImgHeight);
    hd=getPetiteImage(0,moitieW,inputGrey,inputImgWidth,inputImgHeight,searchGrey,moitieW,moitieH+searchImgHeight);
    bg=getPetiteImage(moitieH,0,inputGrey,inputImgWidth,inputImgHeight,searchGrey,moitieW+searchImgWidth,moitieH);
    bd=getPetiteImage(moitieH,moitieW,inputGrey,inputImgWidth,inputImgHeight,searchGrey,moitieW,moitieH);

                int finalResult[NBCOEURS][3];

   #pragma omp parallel sections
    {   
        
            
            
        #pragma omp section
             {
            int result[3];
            test(hg,(moitieW+searchImgWidth),(moitieH+searchImgHeight),searchGrey,searchImgWidth,searchImgHeight,&result[0]);
            finalResult[omp_get_thread_num()][0]=result[0];       
            finalResult[omp_get_thread_num()][1]=result[1]; 
            finalResult[omp_get_thread_num()][2]=result[2]; 
        }
        #pragma omp section
        {
            int result[3];
            test(hd,(moitieW),(moitieH+searchImgHeight),searchGrey,searchImgWidth,searchImgHeight,&result[0]);
            finalResult[omp_get_thread_num()][0]=result[0];       
            finalResult[omp_get_thread_num()][1]=result[1]; 
            finalResult[omp_get_thread_num()][2]=result[2]+moitieW; 

        }
        #pragma omp section
        {    

            int result[3];
            test(bg,(moitieW+searchImgWidth),(moitieH),searchGrey,searchImgWidth,searchImgHeight,&result[0]);
            finalResult[omp_get_thread_num()][0]=result[0];       
            finalResult[omp_get_thread_num()][1]=result[1]+moitieH; 
            finalResult[omp_get_thread_num()][2]=result[2];      


        }
        #pragma omp section
        {
            int result[3];
            test(bd,(moitieW),(moitieH),searchGrey,searchImgWidth,searchImgHeight,&result[0]);
            finalResult[omp_get_thread_num()][0]=result[0];       
            finalResult[omp_get_thread_num()][1]=result[1]+moitieH; 
            finalResult[omp_get_thread_num()][2]=result[2]+moitieW; 

        }
    }


    int minDiff=finalResult[0][0];
    int minI=finalResult[0][1];
    int minJ=finalResult[0][2];
    for(int i=1;i<NBCOEURS;i++){
        if(finalResult[i][0]<minDiff){
            minDiff=finalResult[i][0];
            minI=finalResult[i][1];
            minJ=finalResult[i][2];
        }
    }
    encadrerEnRouge(minI,minJ,inputImg,inputImgWidth,inputImgHeight,searchImgWidth,searchImgHeight);

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
    #pragma omp for schedule(dynamic)
    for (int i=0;i<inputImgHeight;i++){
        for(int j=0;j<inputImgWidth;j++){
            if(inputImgHeight-i>=searchImgHeight && inputImgWidth-j>=searchImgWidth ){
                unsigned char * petiteImage = getPetiteImage(i,j,imgSource,inputImgWidth,inputImgHeight,searchImg,searchImgWidth,searchImgHeight);
                int differenceCourante=compareImage(petiteImage,searchImg,searchImgHeight,searchImgWidth);
                 if(i==81 && j==664){
                    printf("ici:%d\n",imgSource[0]);
                }
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
        printf(" fin %d %d \n",indiceI,indiceJ);
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