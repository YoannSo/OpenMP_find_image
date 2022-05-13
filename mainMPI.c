#include <mpi.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib_stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib_stb_image/stb_image_write.h"
#include <omp.h>

unsigned char * rgb_to_grey(unsigned char* input_rgb,int size);
int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight);
int workerCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight);

int main(int argc, char** argv) {
   

    if (argc != 3)
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
    unsigned char *inputGrey = (unsigned char *)malloc(inputImgHeight*inputImgWidth*3  * sizeof(unsigned char));
    unsigned char *searchGrey = (unsigned char *)malloc(searchImgHeight*searchImgWidth*3  * sizeof(unsigned char));
    inputGrey=rgb_to_grey(inputImg,inputImgHeight*inputImgWidth*3);
    searchGrey=rgb_to_grey(searchImg,searchImgHeight*searchImgWidth*3);



    printf("Input image %s: %dx%d\n", inputImgPath, inputImgWidth, inputImgHeight);
    printf("%d %D \n",inputImgWidth,inputImgHeight);



// Initialize the MPI environment
MPI_Init(NULL, NULL);
/*MPI_SEND(inputImgWidth,1,MPI_INT,0,MPI_COMM_WORLD);
MPI_SEND(inputImgHeight,1,MPI_INT,0,MPI_COMM_WORLD);
MPI_SEND(inputImg,inputImgHeight*inputImgWidth*3,MPI_UNSIGNED_CHAR,0,MPI_COMM_WORLD);
*/
// Get the number of processes
int world_size;
MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// Get the rank of the process
int world_rank;
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


if(world_rank==0){

 
    masterCode(inputGrey,inputImgWidth,inputImgHeight,searchGrey,searchImgWidth,searchImgHeight);
}
else workerCode(inputGrey,inputImgWidth,inputImgHeight,searchGrey,searchImgWidth,searchImgHeight);



// Finalize the MPI environment.
stbi_image_free(inputImg); 
stbi_image_free(searchImg); 
MPI_Finalize();
return 0;
}

unsigned char * rgb_to_grey(unsigned char* input_rgb,int size){
    unsigned char *grey_opti=(unsigned char *)malloc(size   * sizeof(unsigned char));
         int indexGrey=0;
    for(int i=0;i<size;i+=3){
        float value=0.299*input_rgb[i]+0.587*input_rgb[i+1]+0.114*input_rgb[i+2];
            grey_opti[indexGrey]=value;
            grey_opti[indexGrey+1]=value;
            grey_opti[indexGrey+2]=value;
            indexGrey+=3;
    }
    return grey_opti;
}

int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight){
    printf("je suis master %d %d\n",inputHeight,searchWidth);
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    
    int nbLigneAFaire=inputHeight-searchHeight;
    int nbRecv=0;
    int who;
    int whoLast=nprocs-1;
    int task;
    int idLine=0;
    if(whoLast>nbLigneAFaire) whoLast=nbLigneAFaire;
    for(who=1;who<whoLast;who++){
        task=idLine;
        MPI_Send(&task,1,MPI_INT,who,0,MPI_COMM_WORLD);
        printf("%d \n",who);
        idLine++;
    }
}

int workerCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight){
    int i,rank;
    long long answer;
    int idLine;
    MPI_Status status;
    int count=0;


    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    printf("je susi worker %d %d \n",searchHeight,rank);

    MPI_Recv(&idLine,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

    printf("je dois faire la ligne %d et je suis %d",rank,idLine);
}