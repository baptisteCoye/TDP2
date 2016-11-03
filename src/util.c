#include"util.h"

#define G 6.67

double distance(particule A, particule B){
  return sqrt((A.px - B.px)*(A.px - B.px) + (A.py - B.py)*(A.py - B.py));
}

vecteur force_interaction(particule A, particule B){
  vecteur u, res;
  double dAB = distance(A, B);

  u.x = (B.px-A.px)/dAB;
  u.y = (B.py-A.py)/dAB;

  res.x = (G * A.m * B.m / (dAB * dAB)) * u.x;
  res.y = (G * A.m * B.m / (dAB * dAB)) * u.y;

  return res;
}

int readData(char* filename, int nbProc, int myRank, particule ** data, int * nbPart, MPI_Datatype PARTICULE){
  particule * buffer;
  int i, j, k; 
  MPI_Status status;
  FILE * file;
  int nbPartPerProc;
  int err;

  if (myRank == 0){
    file = fopen(filename, "r");
    if (file == NULL){
      fprintf(stderr, "erreur lors de l'ouverture du fichier.\n");
      return -1;
    }
    
    err = fscanf(file, "%d\n", nbPart);
    if (err == EOF)
      return -1;
    
    if (*nbPart < nbProc){
      fprintf(stderr, "Pas assez de particules pour occuper tous les processeurs. Diminuez le nombre de processeurs.\n");
      return -1;
    }
    if ((*nbPart % nbProc) != 0){
      fprintf(stderr, "Le nombre de particules n'est pas un multiple du nombre de processeurs, le cas n'est pas géré.\n");
      return -1;
    }
  }

  nbPartPerProc = *nbPart / nbProc;

  MPI_Bcast(nbPart, 1, MPI_INT, 0, MPI_COMM_WORLD);
  *(data) = malloc(sizeof(particule) * nbPartPerProc);

  if (myRank == 0){
    for (k = 0; k < nbProc - 1; k++){

      for (i = 0; i < nbPartPerProc; i++){
	err = fscanf(file, "%lf %lf %lf %lf %lf\n", &((*data)[i].m), &((*data)[i].px), &((*data)[i].py), &((*data)[i].vx), &((*data)[i].vy));
	if (err == EOF)
	  return -1;
      }
      MPI_Send(data, nbPartPerProc, PARTICULE, i+1, 100, MPI_COMM_WORLD);

    }

    for (i = 0; i < nbPartPerProc; i++){
      err = fscanf(file, "%lf %lf %lf %lf %lf\n", &((*data)[i].m), &((*data)[i].px), &((*data)[i].py), &((*data)[i].vx), &((*data)[i].vy));
      if (err == EOF)
	return -1;
    }
    
  } else {
    MPI_Recv(data, nbPartPerProc, PARTICULE, 0, 100, MPI_COMM_WORLD, &status);
  }

  fclose(file);
  return 0;
}

void calcul_local(vecteur* force, particule* data, int N){

  vecteur tmp;

  for (int i = 0; i < N; i++){
    for (int j = i+1; j < N; j++){
      if (i != j){
	tmp = force_interaction(data[i], data[j]);
	force[i].x += tmp.x;
	force[i].y += tmp.y;
	force[j].x -= tmp.x;
	force[j].y -= tmp.y;
      }
    }
  }
}

void calcul_lointain(vecteur* force, particule* buffer, particule* data, int N){
  vecteur tmp;

  for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++){
      tmp = force_interaction(data[i], buffer[j]);
      force[i].x += tmp.x;
      force[i].y += tmp.y;
    }
  }
}

void copier(particule* buffer, particule* data, int N){
  for (int i = 0; i < N; i++){
    buffer[i] = data[i];
  }
}

void move_particules(particule * data, vecteur * force, int N, double dt){
  for (int i = 0; i < N; i++){
    data[i].ax = force[i].x / data[i].m;
    data[i].ay = force[i].y / data[i].m;

    data[i].px += data[i].vx*dt + data[i].ax*dt*dt/2;
    data[i].py += data[i].vy*dt + data[i].ay*dt*dt/2;

    data[i].vx += data[i].ax*dt;
    data[i].vy += data[i].ay*dt;
  }
}

int save_results(particule * data, int N, char * filename, int nbProc, int myRank, MPI_Datatype PARTICULE){
  int i, j, k; 
  MPI_Status status;
  FILE * file;

  if (myRank == 0){
    particule * buffer = malloc(sizeof(buffer[0]) * N);
    file = fopen(filename, "w+");
    if (file == NULL){
      fprintf(stderr, "erreur lors de l'ouverture du fichier.\n");
      return -1;
    }
    
    fprintf(file, "%d\n", N*nbProc);

    for (i = 0; i < N; i++){
      fprintf(file, "%lf %lf %lf %lf %lf\n", data[i].m, data[i].px, data[i].py, data[i].vx, data[i].vy);
    }

    for (k = 0; k < nbProc - 1; k++){

      for (i = 0; i < N; i++){
	MPI_Recv(buffer, N, PARTICULE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	fprintf(file, "%lf %lf %lf %lf %lf\n", buffer[i].m, buffer[i].px, buffer[i].py, buffer[i].vx, buffer[i].vy);
      }

    }
    
  } else {
    MPI_Send(data, N, PARTICULE, 0, 100, MPI_COMM_WORLD);
  }

  fclose(file);
  return 0;
}
