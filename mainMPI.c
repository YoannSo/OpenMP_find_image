#include <mpi.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
#include <omp.h>
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
unsigned char * rgb_to_grey(unsigned char* input_rgb,int size);
int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight,int* finalResult);
int workerCode();

int main(int argc, char** argv) {
   

   

   
// Initialize the MPI environment
MPI_Init(NULL, NULL);
/*MPI_SEND(inputImgWidth,1,MPI_INT,0,MPI_COMM_WORLD);
MPI_SEND(inputImgHeight,1,MPI_INT,0,MPI_COMM_WORLD);
*/
// Get the number of processes
int world_size;
MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// Get the rank of the process
int world_rank;
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


if(world_rank==0){

     if (argc != 3)
    {
        printf("Invalid arguments !\n");
        return EXIT_FAILURE;
    }
    // Get image paths from arguments.
    const char *inputImgPath = argv[1];
    const char *searchImgPath = argv[2];
    int searchImgWidth;
    int searchImgHeight;
    int inputImgWidth;
    int inputImgHeight;
    
    
    // ==================================== Loading input image.
   
    
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
 
    unsigned char *searchImg = stbi_load(searchImgPath, &searchImgWidth, &searchImgHeight, &dummyNbChannels, 3);
    if (searchImg == NULL)
    {
        printf("Cannot load image %s", searchImgPath);
        return EXIT_FAILURE;
    }
    printf("Search image %s: %dx%d\n", searchImgPath, searchImgWidth, searchImgHeight);


    unsigned char* inputGrey = (unsigned char *)malloc(inputImgHeight*inputImgWidth * sizeof(unsigned char));
    unsigned char* searchGrey = (unsigned char *)malloc(searchImgHeight*searchImgWidth  * sizeof(unsigned char));
    int i,j; 
     inputGrey=rgb_to_grey(inputImg,inputImgHeight*inputImgWidth);
     searchGrey=rgb_to_grey(searchImg,searchImgHeight*searchImgWidth);

    int result[3];
    masterCode(inputGrey,inputImgWidth,inputImgHeight,searchGrey,searchImgWidth,searchImgHeight,&result[0]);
    encadrerEnRouge(result[1],result[2],inputImg,inputImgWidth,inputImgHeight,searchImgWidth,searchImgHeight);

    // ====================================  Save example: save a copy of 'inputImg'
     unsigned char *saveExample = (unsigned char *)malloc(inputImgWidth * inputImgHeight  * 3*sizeof(unsigned char));
    memcpy( saveExample, inputImg, inputImgWidth * inputImgHeight*3  * sizeof(unsigned char) );

    stbi_write_png("img/save_example.png", inputImgWidth, inputImgHeight, 3, saveExample, inputImgWidth * 3);

    free(saveExample);
    stbi_image_free(inputImg); 
    stbi_image_free(searchImg); 

    printf("Good bye!\n");

}
 

   

if(world_rank!=0) workerCode();



// Finalize the MPI environment.

MPI_Finalize();
return 0;
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

int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight,int* finalResult){
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    printf("je suis master %d %d\n",inputHeight,searchWidth);
    int who;
    int whoLast=nprocs-1;

    for(who=1;who<=whoLast;who++){
        MPI_Send(&inputWidth,1,MPI_INT,who,0,MPI_COMM_WORLD);
        MPI_Send(&inputHeight,1,MPI_INT,who,0,MPI_COMM_WORLD);
        MPI_Send(&searchWidth,1,MPI_INT,who,0,MPI_COMM_WORLD);
        MPI_Send(&searchHeight,1,MPI_INT,who,0,MPI_COMM_WORLD);

        MPI_Send(&inputIMG[0],inputWidth*inputHeight,MPI_UNSIGNED_CHAR,who,0,MPI_COMM_WORLD);
        MPI_Send(&searchIMG[0],searchWidth*searchHeight,MPI_UNSIGNED_CHAR,who,0,MPI_COMM_WORLD);

    }
    int nbLigneAFaire=inputHeight-searchHeight;
    printf("%d\n",inputIMG[0]);
    int nbRecv=0;
    int task;
    int idLine=0;
    int result[3];
    int minDiff=99999999999;
    int minI,minJ;
    if(whoLast>nbLigneAFaire) whoLast=nbLigneAFaire;
    for(who=1;who<=whoLast;who++){
        task=idLine;
        MPI_Send(&task,1,MPI_INT,who,2,MPI_COMM_WORLD);
        idLine++;
    }
        MPI_Status fromWhom;

    while(nbRecv<nbLigneAFaire){
        MPI_Recv(&result[0],3,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&fromWhom);
        nbRecv++;
        if(result[0]<minDiff){
            minDiff=result[0];
            minI=result[1];
            minJ=result[2];
        }
        task=idLine;
        MPI_Send(&task,1,MPI_INT,fromWhom.MPI_SOURCE,2,MPI_COMM_WORLD);
        idLine++;
    }
     for(who=1;who<=whoLast;who++){
        MPI_Send(&task,1,MPI_INT,who,7,MPI_COMM_WORLD);
    }
    finalResult[0]=minDiff;
    finalResult[1]=minI;
    finalResult[2]=minJ;
    printf("%d %d %d\n",minI,minJ,minDiff);
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
int*  findImage(int idLine,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* result){
    int indiceI=-1;
    int indiceJ=-1;
    int differenceMin=9999999999;
    int indices [2];
        
        for(int j=0;j<inputImgWidth;j++){
            if(inputImgHeight-idLine>=searchImgHeight && inputImgWidth-j>=searchImgWidth ){
                unsigned char * petiteImage = getPetiteImage(idLine,j,imgSource,inputImgWidth,inputImgHeight,searchImg,searchImgWidth,searchImgHeight);
                int differenceCourante=compareImage(petiteImage,searchImg,searchImgHeight,searchImgWidth);
                if(idLine==81 && j==664){
                    printf("ici:%d\n",imgSource[0]);
                }
                if(differenceCourante<differenceMin){
                    differenceMin=differenceCourante;
                    indiceJ=j;
                    indiceI=idLine;
                } 
                free(petiteImage);
            }
                        

        }

    
    if(indiceI!=-1){
        result[0]=differenceMin;
        result[1]=indiceI;
        result[2]=indiceJ;
    }
}

int workerCode(){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int searchWidth;
    int searchHeight;
    int inputWidth;
    int inputHeight;
    

    MPI_Recv(&inputWidth,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(&inputHeight,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(&searchWidth,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(&searchHeight,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    unsigned char* inputGrey = (unsigned char *)malloc(inputHeight*inputWidth * sizeof(unsigned char));
    unsigned char* searchGrey = (unsigned char *)malloc(searchWidth*searchHeight  * sizeof(unsigned char));

    MPI_Recv(&inputGrey[0],inputHeight*inputWidth,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(&searchGrey[0],searchWidth*searchHeight,MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);


    long long answer;
    int idLine;
    MPI_Status status;
    int count=0;


    while(1){
        MPI_Recv(&idLine,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        if(status.MPI_TAG==2){
            int result[3];
            findImage(idLine,inputGrey,inputWidth,inputHeight,searchGrey,searchWidth,searchHeight,&result[0]);
            MPI_Send(&result[0],3,MPI_INT,0,0,MPI_COMM_WORLD);
        }
        else break;
    }
}
