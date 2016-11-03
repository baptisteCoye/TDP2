#include <stdio.h>
#include <stdlib.h>

#include "datatype.h"
#include "util.h"
#include "TDPConfig.h"

#define FILENAME "../tests/data/file_test.txt"
#define NB_ITER 100

int main(int argc, void** argv){
  int N;
  particule * data;
  vecteur * forces;
  double * distMin;
  double dt;
  int err; 

  FILE * readfile = fopen(FILENAME, "r");
  if (readfile == NULL){
    fprintf(stderr, "Erreur lors de la lecture du readfile. Fermeture du programme de test.\n");
    return EXIT_FAILURE;
  }

  err = fscanf(readfile, "%d\n", &N);
  if (err < 0)
    fprintf(stderr, "Erreur lors de la lecture dans le fichier. err = :%d:\n", err);

#if VERBOSE >= 1
  printf("N = %d\n", N);
#endif

  data = malloc(sizeof(particule) * N);
  forces = malloc(sizeof(vecteur) * N);
  distMin = malloc(sizeof(double) * N);

  for (int i = 0; i < N; i++){
    err = fscanf(readfile, "%lf %lf %lf %lf %lf %lf %lf\n", &(data[i].m), &(data[i].px), &(data[i].py), &(data[i].vx), &(data[i].vy), &(data[i].ax), &(data[i].ay));
    if (err < 0)
      fprintf(stderr, "Erreur lors de la lecture dans le fichier. err = :%d:\n", err);
  }

  fclose(readfile);

  for (int i = 0; i < NB_ITER; i++){
#if VERBOSE >= 1
    printf("iteration %d\n", i);
#endif

    dt = DT_MAX;

#if VERBOSE >= 1
    printf("     calcul des forces ...\n");
#endif

    calcul_local(forces, data, N, distMin);

    double dtTmp;

#if VERBOSE >= 1
    printf("     determination du dt ... ");
#endif

    for (int i = 0; i < N; i++){
      dtTmp = determine_dt(data[i], forces[i], distMin[i]);
      if (dtTmp < dt)
	dt = dtTmp;
    }
    if (dt < DT_MIN)
      dt = DT_MIN;

#if VERBOSE >= 2
    printf("%lf\n", dt);
#endif

#if VERBOSE >= 1
    printf("     deplacement des particules\n");
#endif

    move_particules(data, forces, N, dt);

#if VERBOSE >= 1
    printf("     sauvegarde des donnees\n");
#endif

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

  free(data); free(forces); free(distMin);
  
  return 0;
}
