#include "utils.h"
#include <stdio.h>
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
    unsigned char *inputGrey=getGrey(inputImgPath,&inputWidth,&inputHeight);
    unsigned char *searchGrey=getGrey(searchImgPath,&searchWidth,&searchHeight);

    printf("%d %d %d %d\n",inputNormal[5],searchGrey[10],inputGrey[0]);

    int i,j;
    findImageInImage(inputGrey,inputWidth,inputHeight,searchGrey,searchWidth,searchHeight,&i,&j);
    printf("%d %d  \n",i,j);
    encadrerEnRouge(i,j,inputNormal,inputWidth,inputHeight,searchWidth,searchHeight);

    save(inputNormal,inputWidth,inputHeight);
   


    return 1;
}
