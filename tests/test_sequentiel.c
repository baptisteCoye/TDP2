#include <stdio.h>
#include <stdlib.h>

#include "datatype.h"
#include "util.h"

#define dt 1 
#define FILENAME "../tests/data/file_test.txt"
#define NB_ITER 100

int main(int argc, void** argv){
  int N;
  particule* data;
  vecteur* forces;

  FILE * readfile = fopen(FILENAME, "r");
  if (readfile == NULL){
    fprintf(stderr, "Erreur lors de la lecture du readfile. Fermeture du programme de test.\n");
    return EXIT_FAILURE;
  }

  fscanf(readfile, "%d\n", &N);
  data = malloc(sizeof(particule) * N);
  forces = malloc(sizeof(vecteur) * N);

  for (int i = 0; i < N; i++){
    fscanf(readfile, "%lf %lf %lf %lf %lf\n", &(data[i].m), &(data[i].px), &(data[i].py), &(data[i].vx), &(data[i].vy));
    printf(" line read : %lf %lf %lf %lf %lf\n", (data[i].m), (data[i].px), (data[i].py), (data[i].vx), (data[i].vy));
  }

  fclose(readfile);

  for (int i = 0; i < NB_ITER; i++){
    calcul_local(forces, data, N);
    if (i == 0){
      for (int i = 0; i < N; i++){
	printf(" %lf %lf %lf %lf %lf\n", (data[i].m), (data[i].px), (data[i].py), (data[i].vx), (data[i].vy));
      }
      printf("\n");
    }

    move_particules(data, forces, N, dt);
    if (i == 0){
      for (int i = 0; i < N; i++){
	printf(" %lf %lf %lf %lf %lf\n", (data[i].m), (data[i].px), (data[i].py), (data[i].vx), (data[i].vy));
      }
      printf("\n");
    }

    char * filename = malloc(sizeof(char) * 20);
    snprintf(filename, 20, "save_seq_%d.txt", i);    

    FILE * writefile = fopen(filename, "w+");
    if (writefile == NULL) {
      fprintf(stderr, "Erreur lors de l'ouverture du fichier %s en ecriture. Fermeture du programme.\n", filename);
      return EXIT_FAILURE;
    }
      
    
    fprintf(writefile, "%d\n", N);
    for (int i = 0; i < N; i++)
      fprintf(writefile, "%lf %lf %lf %lf %lf\n", data[i].m, data[i].px, data[i].py, data[i].vx, data[i].vy);
    fclose(writefile);
    free(filename);
  }

  return 0;
}

  
