#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
#include <omp.h>
#include "utils.h"
#define NBCOEURS 6


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


            int nbSub=NBCOEURS;
            if(nbSub%2!=0){
                nbSub++;
            }
            int nbLigne=nbSub*0.5;
            int tailleW=inputImgWidth/nbLigne;
            int tailleH=inputImgHeight*0.5;
            int finalResult[nbSub][3];
   #pragma omp parallel sections
    {   
        
            omp_set_num_threads(NBCOEURS);
            #pragma omp section
            {


                for(int i=0;i<NBCOEURS;i++){
                    #pragma omp task
                    {
                        int result[3];
                        printf("%d \n",omp_get_thread_num());
                        if(i<nbLigne){
                            if(i==nbLigne-1){
                                unsigned char *petiteimage = (unsigned char *)malloc(tailleW*(tailleH+searchImgHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(0,tailleW*i,inputGrey,inputImgWidth,inputImgHeight,searchGrey,tailleW,tailleH+searchImgHeight);
                                test(petiteimage,tailleW,tailleH+searchImgHeight,searchGrey,searchImgWidth,searchImgHeight,&result[0]);                                
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]; 
                                finalResult[i][2]=result[2]+tailleW*i;
                                free(petiteimage);

                            }
                            else{
                                unsigned char *petiteimage = (unsigned char *)malloc((tailleW+searchImgWidth)*(tailleH+searchImgHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(0,i*tailleW,inputGrey,inputImgWidth,inputImgHeight,searchGrey,tailleW+searchImgWidth,tailleH+searchImgHeight);
                                test(petiteimage,tailleW+searchImgWidth,tailleH+searchImgHeight,searchGrey,searchImgWidth,searchImgHeight,&result[0]);
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]; 
                                finalResult[i][2]=result[2]+i*tailleW;
                                free(petiteimage);
                            }
                     
                        }
                        else{
                            int index=i%nbLigne;
                            if(i==nbSub-1){
                                unsigned char *petiteimage = (unsigned char *)malloc(tailleW*tailleH * sizeof(unsigned char));
                                petiteimage=getPetiteImage(tailleH,index*tailleW,inputGrey,inputImgWidth,inputImgHeight,searchGrey,tailleW,tailleH);
                                test(petiteimage,tailleW,tailleH,searchGrey,searchImgWidth,searchImgHeight,&result[0]);
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]+tailleH; 
                                finalResult[i][2]=result[2]+index*tailleW;
                                free(petiteimage);

                            }
                            else{
                                unsigned char *petiteimage = (unsigned char *)malloc((tailleW+searchImgWidth)*(tailleH+searchImgHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(moitieH,index*tailleW,inputGrey,inputImgWidth,inputImgHeight,searchGrey,tailleW+searchImgWidth,tailleH+searchImgHeight);
                                test(petiteimage,tailleW+searchImgWidth,tailleH,searchGrey,searchImgWidth,searchImgHeight,&result[0]);
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]+tailleH; 
                                finalResult[i][2]=result[2]+index*tailleW;
                                free(petiteimage);

                            }
                     
                        }

                    } 
                }
            }
        
    }

    int minDiff=finalResult[0][0];
    int minI=finalResult[0][1];
    int minJ=finalResult[0][2];
    for(int i=1;i<nbSub;i++){
        printf("%d %d %d \n",finalResult[i][0],finalResult[i][1],finalResult[i][2]);
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
