/*#include "workCode.h"
#include <mpi/mpi.h>

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
}*/