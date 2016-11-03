#include"util.h"

#define G 6.67

double distance(particule A, particule B){
  return sqrt((A.px - B.px)*(A.px - B.px) + (A.py - B.py)*(A.py - B.py));
}

vecteur force_interaction(particule A, particule B, double* distanceMinTmp){
  vecteur u, res;
  double dAB = distance(A, B);
  *distanceMinTmp = dAB;
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

void calcul_local(vecteur* force, particule* data, int N, double* distMin){

  vecteur tmp;

  for (int i = 0; i < N; i++){
    distMin[i] = -1;
  }

  for (int i = 0; i < N; i++){
    for (int j = i+1; j < N; j++){
      if (i != j){
	double distTmp;
        tmp = force_interaction(data[i], data[j], &distTmp);
        if (distTmp < distMin[i] || distMin[i] == -1){
          distMin[i] = distTmp;
        }
        if (distTmp < distMin[j] || distMin[j] == -1){
          distMin[j] = distTmp;
        }
        force[i].x += tmp.x;
	    force[i].y += tmp.y;
	    force[j].x -= tmp.x;
	    force[j].y -= tmp.y;
      }
    }
  }
}

void calcul_lointain(vecteur* force, particule* buffer, particule* data, int N, double* distMin){
  vecteur tmp;
  double distTmp;

  for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++){
      tmp = force_interaction(data[i], buffer[j], &distTmp);
      force[i].x += tmp.x;
      force[i].y += tmp.y;
      if (distTmp < distMin[i] || distMin[i] == -1){
        distMin[i] = distTmp;
      }
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

double max(double a, double b){
    if(a < b) return b;
    else return a;
}

double determine_dt(particule data, vecteur force, double distMin){
    double dtx;
    double dty;
    double discrx = data.vx*data.vx + 4*data.ax*distMin;
    double discrInvx = data.vx*data.vx - 4*data.ax*distMin;
    double discry = data.vy*data.vy + 4*data.ay*distMin;
    double discrInvy = data.vy*data.vy - 4*data.ay*distMin;

    if (discrx > 0 && discrInvx > 0) {
        dtx = max(((-data.vx + sqrt(discrx)) / (2 * data.ax)),((-data.vx + sqrt(discrx))/(2*data.ax)));
    }
    else if (discrx > discrInvx){
        dtx = (data.vx + sqrt(discrx))/(2*data.ax);
    }
    else dtx = (data.vx + sqrt(discrInvx))/(2*data.ax);

    if (discry > 0 && discrInvy > 0) {
        dty = max(((-data.vy + sqrt(discry)) / (2 * data.ay)),((-data.vy + sqrt(discry))/(2*data.ay)));
    }

    else if (discry > discrInvy) {
      dty = (-data.vy+sqrt(discry))/(2*data.ay);
    }
    else dty = (data.vy + sqrt(discrInvy))/(2*data.ay);

    if(dtx < dty)
      return dtx;
    else 
      return dty;
}

double determine_dt_forall(particule* data, vecteur* force, int N, double* distMin, int nbProc){
    int i;
    double dt = DT_MAX;
    double dtTmp;
    double dtTot;

    for (i = 0; i < N; i++){
      dtTmp = determine_dt(data[i], force[i], distMin[i]/sqrt(2));
      if (dtTmp < dt){
	dt = dtTmp;
      }
    }
    /* if (dt < DT_MIN) */
    /*   dt = DT_MIN; */

    if (nbProc > 1)
      MPI_Allreduce(&dt, &dtTot, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    else 
      dtTot = dt;

    return dtTot;
}
