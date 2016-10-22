#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "datatype.h"
#include "util.h"

#define G 6.67
#define TAG 100

int main(){

  int err;
  int nbProc, rank;
  particule * data;
  particule * buffer[2];
  double * force;
  int nbParticules;
  int tag;
  MPI_Status status;
  MPI_Request sendRequests[2];
  MPI_Request recvRequests[2];
  int mesParticules;

  MPI_Init(NULL,NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  particule p[1];

  MPI_Datatype type[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
  int blocklen[3] = {1,1,1};

  MPI_Aint disp[3];

  MPI_Aint i1,i2;

  MPI_Get_address(&p[0], &i1);
  MPI_Get_address(&p[0].m, &i2); disp[0] = i2-i1;
  MPI_Get_address(&p[0].px, &i2); disp[1] = i2-i1;
  MPI_Get_address(&p[0].py, &i2); disp[2] = i2-i1;
  
  MPI_Datatype V_PARTICULE;

  err =  MPI_Type_struct(3, blocklen, disp, type, &V_PARTICULE);
  if (err != 0){
    fprintf(stderr, "erreur lors du MPI_Type_struct() : err = %d", err);
    return EXIT_FAILURE;
  }

  err = MPI_Type_commit(&V_PARTICULE);
  if (err != 0){
    fprintf(stderr, "erreur lors du MPI_Type_commit() : err = %d", err);
    return EXIT_FAILURE;
  }

  nbParticules = readData(&data, NULL, rank);

  mesParticules = nbParticules / nbProc;

  buffer[0] = malloc(sizeof(particule) * mesParticules);
  buffer[1] = malloc(sizeof(particule) * mesParticules);
  force = malloc(sizeof(double) * mesParticules);
  
  for (int i = 0; i < mesParticules; i++){
    force[i] = 0.0;
  }

  copier(buffer[0], data, mesParticules);

  // Envoie depuis le buffer 0
  MPI_Send_init(buffer[0], mesParticules, V_PARTICULE, (rank+1)%size, TAG, MPI_COMM_WORLD, &sendRequests[0]);
  // Envoie depuis le buffer 1
  MPI_Send_init(buffer[1], mesParticules, V_PARTICULE, (rank+1)%size, TAG, MPI_COMM_WORLD, &sendRequests[1]);

  // Réception sur le buffer 0
  MPI_Recv_init(buffer[0], mesParticules, V_PARTICULE, (rank-1)%size, TAG, MPI_COMM_WORLD, &recvRequests[0]);
  // Réception sur le buffer 1
  MPI_Recv_init(buffer[1], mesParticules, V_PARTICULE, (rank-1)%size, TAG, MPI_COMM_WORLD, &recvRequests[1]);

  MPI_Start(&sendRequest[0]);
  MPI_Start(&recvRequest[1]);

  calcul_local(force, data, mesParticules);

  MPI_Wait(&sendRequest[0], NULL);
  MPI_Wait(&recvRequest[1], NULL);

  for (int i = 1; i < nbProc; i++){

    calcul_lointain(force, buffer[i%2], data, mesParticules);

    err = MPI_Ssend(buffer, mesParticules, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    if (err != 0){
      fprintf(stderr, "erreur :%d: sur le mpi_ssend. fermeture du programme\n", err);
      return EXIT_FAILURE;
    }
    
    err = MPI_RCV(buffer, mesParticules, MPI_INT, (rank-1)%size, tag, MPI_COMM_WORLD, &status);
    
  MPI_Finalize();

}
