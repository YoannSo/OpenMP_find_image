#include <mpi.h>
#include "utils.h"

int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight,int* finalResult);
int workerCode();

int main(int argc, char** argv) {
   

   

   
// Initialize the MPI environment
MPI_Init(NULL, NULL);
/*MPI_SEND(inputWidth,1,MPI_INT,0,MPI_COMM_WORLD);
MPI_SEND(inputHeight,1,MPI_INT,0,MPI_COMM_WORLD);
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
        return 0;
    }
    // Get image paths from arguments.
    const char *inputImgPath = argv[1];
    const char *searchImgPath = argv[2];
    
    int inputWidth,inputHeight,searchWidth,searchHeight;
    unsigned char* inputNormal=getNormal(inputImgPath,&inputWidth,&inputHeight);
    unsigned char *inputGrey=getGrey(inputImgPath,&inputWidth,&inputHeight);
    unsigned char *searchGrey=getGrey(searchImgPath,&searchWidth,&searchHeight);


    int result[3];
    masterCode(inputGrey,inputWidth,inputHeight,searchGrey,searchWidth,searchHeight,&result[0]);
    encadrerEnRouge(result[1],result[2],inputNormal,inputWidth,inputHeight,searchWidth,searchHeight);

    save(inputNormal,inputWidth,inputHeight);
    printf("Good bye!\n");

}
if(world_rank!=0) workerCode();

// Finalize the MPI environment.
MPI_Finalize();
return 0;
}

int masterCode(unsigned char * inputIMG,int inputWidth,int inputHeight,unsigned char * searchIMG,int searchWidth,int searchHeight,int* finalResult){
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
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
