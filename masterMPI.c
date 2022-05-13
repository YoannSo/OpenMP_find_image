/*#include "workCode.h"
#include <mpi/mpi.h>
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
}*/