#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Invalid arguments !\n");
        return 0;
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
    if(inputGrey==NULL){
        return 1;
    }
    unsigned char *searchGrey=getGrey(searchImgPath,&searchWidth,&searchHeight);
    if(searchGrey==NULL){
        return 1;
    }
    int i,j;
    findImageInImage(inputGrey,inputWidth,inputHeight,searchGrey,searchWidth,searchHeight,&i,&j);
    encadrerEnRouge(i,j,inputNormal,inputWidth,inputHeight,searchWidth,searchHeight);

    save(inputNormal,inputWidth,inputHeight);

    free(searchGrey);
    free(inputGrey);
    return 0;
}
