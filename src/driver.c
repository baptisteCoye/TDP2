#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "datatype.h"
#include "util.h"

#define G 6.67
#define TAG 100

int main(int argc, char **argv){

  /*  les arguments doivent etre argv[1] = nb_iterations & argv[2] = fileName  */
  if(argc != 3){
    fprintf(stderr, "Le programme doit contenir exactement deux arguments : le nombre desire d'iterations puis le nom du fichier ou recuperer les donnees.\n");
    return EXIT_FAILURE;
  }

  //////////////////////////////////////////////////////////////////
  ///           Initialisation des variables et du MPI           ///
  //////////////////////////////////////////////////////////////////

  int err;
  int nbProc, rank;
  particule * data;
  particule * buffer[2];
  vecteur * force;
  int nbParticules;
  int tag;
  MPI_Status status;
  MPI_Request sendRequests[2];
  MPI_Request recvRequests[2];
  int nbPartPerProc;

  MPI_Init(NULL,NULL);

  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  //////////////////////////////////////////////////////////////////
  ///           Creation du MPI_Datatype V_PARTICULE             ///
  //////////////////////////////////////////////////////////////////

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

  /////////////////////////////////////////////////////////////////
  /// Lecture du fichier et allocation des tableaux et buffers  ///
  /////////////////////////////////////////////////////////////////

  nbParticules = readData(&data, argv[2]);

  nbPartPerProc = nbParticules / nbProc;

  buffer[0] = malloc(sizeof(particule) * nbPartPerProc);
  buffer[1] = malloc(sizeof(particule) * nbPartPerProc);
  force = malloc(sizeof(double) * nbPartPerProc);

  /////////////////////////////////////////////////////////////////
  ///               Creation des requetes MPI                   ///
  /////////////////////////////////////////////////////////////////

  // Envoie depuis le buffer 0
  err = MPI_Send_init(buffer[0], nbPartPerProc, V_PARTICULE, (rank+1)%nbProc, TAG, MPI_COMM_WORLD, &sendRequests[0]);
  fprintf(stderr, "Erreur detectee dans MPI_Send_init 0. err = %d\n", err);
  // Envoie depuis le buffer 1
  err = MPI_Send_init(buffer[1], nbPartPerProc, V_PARTICULE, (rank+1)%nbProc, TAG, MPI_COMM_WORLD, &sendRequests[1]);
  fprintf(stderr, "Erreur detectee dans MPI_Send_init 1. err = %d\n", err);

  // Réception sur le buffer 0
  err = MPI_Recv_init(buffer[0], nbPartPerProc, V_PARTICULE, (rank-1)%nbProc, TAG, MPI_COMM_WORLD, &recvRequests[0]);
  fprintf(stderr, "Erreur detectee dans MPI_Recv_init 0. err = %d\n", err);
  // Réception sur le buffer 1
  err = MPI_Recv_init(buffer[1], nbPartPerProc, V_PARTICULE, (rank-1)%nbProc, TAG, MPI_COMM_WORLD, &recvRequests[1]);
  fprintf(stderr, "Erreur detectee dans MPI_Recv_init 1. err = %d\n", err);
  
  ///////////////////////////////////////////////////////////////////
  ///              Debut des iterations de calcul                 ///
  ///////////////////////////////////////////////////////////////////

  for (int n = 0; n < atoi(argv[1]); n++){

    // Initialisation des forces a 0.
    for (int i = 0; i < nbPartPerProc; i++){
      force[i].x = 0.0;
      force[i].y = 0.0;
    }

    // On copie les donnees possedees par le processus dans son buffer.
    copier(buffer[0], data, nbPartPerProc);

    // On demarre les requetes MPI pour l'envoi et la reception des donnees.
    err = MPI_Start(&sendRequests[0]);
    err = MPI_Start(&recvRequests[1]);

    // Pendant que le MPI gere l'envoi des donnees, on calcule
    // les forces entre les particules dans data et elles memes.
    calcul_local(force, data, nbPartPerProc);

    // Une fois que le calcul est fini, on attend la fin des echanges.
    err = MPI_Wait(&sendRequests[0], NULL);
    err = MPI_Wait(&recvRequests[1], NULL);

    // Debut des iterations de calcul sur les donnees des autres processus.
    for (int i = 1; i < nbProc; i++){

      // Demarrage des requetes MPI sur pour l'envoi et la reception des donnees.
      MPI_Start(&sendRequests[i%2]);
      MPI_Start(&recvRequests[(i+1)%2]);

      // Pendant que le MPI gere l'envoi des donnees, on calcule
      // les forces entre les particules de data et celles reçues precedemment.
      calcul_lointain(force, buffer[i%2], data, nbPartPerProc);

      // Une fois le calcul fini, on attend la fin des echanges.
      MPI_Wait(&sendRequests[i%2], NULL);
      MPI_Wait(&recvRequests[(i+1)%2], NULL);
    }

    // Au bout de nbProc-1 iterations, on a l'ensemble des forces.
    // On applique ces forces aux particules.
    move_particules();
    // On enregistre les resultats.
    save_results();
  }

  /////////////////////////////////////////////////////////////////////
  ///                 Liberation des ressources                     ///
  /////////////////////////////////////////////////////////////////////

  // desallocation
  free(data);
  free(buffer[0]);
  free(buffer[1]);
  free(force);

  // Fin du MPI
  MPI_Type_free(&V_PARTICULE);
  MPI_Finalize();
  return 0;
}
