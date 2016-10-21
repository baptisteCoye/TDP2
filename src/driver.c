#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "datatype.h"

#define G 6.67

int main(){

  int i, j, k, err;
  int nbProc, rank;
  particule * data;
  particule * buffer[2];
  double * force;
  int nbParticules;
  int tag;
  MPI_Status status;
  MPI_Request requests[4];

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

  buffer[0] = malloc(nbProcof(particule) * nbParticules/nbProc);
  buffer[1] = malloc(nbProcof(particule) * nbParticules/nbProc);
  force = malloc(nbProcof(double) * nbParticules/nbProc);
  
  for (int i = 0; i < nbParticules/nbProc; i++){
    force[i] = 0.0;
  }

  copier(buffer[0], data, nbParticules/nbProc);

  calcul_local(force, data, nbParticules/nbProc);

  err = MPI_Send_init(buffer[0], nbParticules/nbProc, MPI_Datatype datatype, int dest,
		    int tag, MPI_Comm comm, MPI_Request *request)

  for (int i = 1; i < nbProc; i++){

    calcul_lointain(force, buffer[i%2], data, nbParticules/nbProc);

    err = MPI_Ssend(buffer, nbParticules/nbProc, MPI_INT, (rank+1)%size, tag, MPI_COMM_WORLD);
    if (err != 0){
      fprintf(stderr, "erreur :%d: sur le mpi_ssend. fermeture du programme\n", err);
      return EXIT_FAILURE;
    }
    
    err = MPI_RCV(buffer, nbParticules/nbProc, MPI_INT, (rank-1)%size, tag, MPI_COMM_WORLD, &status);
    
  MPI_Finalize();

}
