#include <stdio.h>
#include <stdlib.h>
#include "util.h"

#define dt 1 
int main(int argc, void** argv){
  particule* data;
  vecteur* forces;
  int i, j;

  int N = readData(data,argv[1]);
  

//Calcul des forces en séquentiel
  for( i = 0; i < N; i++){
    for( j = i+1; j < N; j++){
      vecteur res;
      res = calcForces(data[i],data[j]);
      forces[i] += res;
      forces[j] -= res;
    }
  }


  
  for ( i = 0; i < N; i++){
    vecteur acceleration;
    veteur newVitesse;
    vecteur newPos;
//Calcul des nouveaux points   
    acceleration.x = force[i].x/data[i].m;
    acceleration.y = force[i].y/data[i].m;
    newVitesse.x = data[i].v.x + acceleration.x*dt;
    newVitesse.y = data[i].v.y + acceleration.y*dt;
    newPos.x = data[i].p.x + data[i].v.x*dt + acceleration.x*dt*dt/2;
    newPos.y = data[i].p.y + data[i].v.y*dt + acceleration.y*dt*dt/2;
    
//affectation des nouvelles positions
    data[i].px = newPos.x;
    data[i].py = newPos.y;
    data[i].vx = newVitesse.x;
    data[i].vy = newVitesse.y;
  }

  return 1;
}

  
