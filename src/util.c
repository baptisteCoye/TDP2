#include"util.h"

vecteur force_interaction(particule A, particule B){
  
}

int readData(particule **_data, char* filename){

}

void calcul_local(vecteur* force, particule* data, int N){

}

void calcul_lointain(vecteur* force, particule* buffer, particule* data, int N){

}

void copier(particule* buffer, particule* data, int N){
  for (int i = 0; i < N; i++){
    buffer[i] = data[i];
  }
}

void move_particules(){}

void save_results(){}
