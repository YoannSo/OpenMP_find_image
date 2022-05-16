#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#define NBCOEURS 6


int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid arguments !\n");
        return 1;
    }
    // Get image paths from arguments.
    const char *inputImgPath = argv[1];
    const char *searchImgPath = argv[2];
    
    int inputWidth,inputHeight,searchWidth,searchHeight;
    unsigned char* inputNormal=getNormal(inputImgPath,&inputWidth,&inputHeight);
    if(inputNormal==NULL){
        return 1;
    }
    unsigned char *inputGrey=getGrey(inputImgPath,&inputWidth,&inputHeight);
     if(inputNormal==NULL){
        return 1;
    }
    unsigned char *searchGrey=getGrey(searchImgPath,&searchWidth,&searchHeight);
     if(inputNormal==NULL){
        return 1;
    }
    int nbSub=NBCOEURS;
    if(nbSub%2!=0){
        nbSub++;
    }
    int nbLigne=nbSub*0.5;
    int tailleW=inputWidth/nbLigne;
    int tailleH=inputHeight*0.5;
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
                        if(i<nbLigne){
                            if(i==nbLigne-1){
                                unsigned char *petiteimage = (unsigned char *)malloc(tailleW*(tailleH+searchHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(0,tailleW*i,inputGrey,inputWidth,inputHeight,searchGrey,tailleW,tailleH+searchHeight);
                                test(petiteimage,tailleW,tailleH+searchHeight,searchGrey,searchWidth,searchHeight,&result[0]);                                
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]; 
                                finalResult[i][2]=result[2]+tailleW*i;
                                free(petiteimage);

                            }
                            else{
                                unsigned char *petiteimage = (unsigned char *)malloc((tailleW+searchWidth)*(tailleH+searchHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(0,i*tailleW,inputGrey,inputWidth,inputHeight,searchGrey,tailleW+searchWidth,tailleH+searchHeight);
                                test(petiteimage,tailleW+searchWidth,tailleH+searchHeight,searchGrey,searchWidth,searchHeight,&result[0]);
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
                                petiteimage=getPetiteImage(tailleH,index*tailleW,inputGrey,inputWidth,inputHeight,searchGrey,tailleW,tailleH);
                                test(petiteimage,tailleW,tailleH,searchGrey,searchWidth,searchHeight,&result[0]);
                                finalResult[i][0]=result[0];       
                                finalResult[i][1]=result[1]+tailleH; 
                                finalResult[i][2]=result[2]+index*tailleW;
                                free(petiteimage);

                            }
                            else{
                                unsigned char *petiteimage = (unsigned char *)malloc((tailleW+searchWidth)*(tailleH+searchHeight) * sizeof(unsigned char));
                                petiteimage=getPetiteImage(tailleH,index*tailleW,inputGrey,inputWidth,inputHeight,searchGrey,tailleW+searchWidth,tailleH+searchHeight);
                                test(petiteimage,tailleW+searchWidth,tailleH,searchGrey,searchWidth,searchHeight,&result[0]);
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
        if(finalResult[i][0]<minDiff){
            minDiff=finalResult[i][0];
            minI=finalResult[i][1];
            minJ=finalResult[i][2];
        }
    }

   
    encadrerEnRouge(minI,minJ,inputNormal,inputWidth,inputHeight,searchWidth,searchHeight);
    save(inputNormal,inputWidth,inputHeight);


    printf("Good bye!\n");
    free(inputGrey);
    free(searchGrey);
    return 0;
}
