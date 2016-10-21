#include"util.h"

vecteur force_interaction(particule A, particule B){
  
}

particule* readData(char* filename, int size){

}

void calcul_local(double* force, particule* data, int N){

}

void calcul_lointain(double* force, particule buffer, particule* data, int N){

}

void copier(particule* buffer, particule* data, int N){
  int i;
  for (i = 0; i < N; i++){
    buffer[i] = data[i];
  }
}
