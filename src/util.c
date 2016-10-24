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

int readData(particule **_data, char* filename){

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
    data.ax = force[i].x / data[i].m;
    data.ay = force[i].y / data[i].m;

    data.px += data.vx*dt + data.ax*dt*dt/2;
    data.py += data.vy*dt + data.ay*dt*dt/2;

    data.vx += data.ax*dt;
    data.vy += data.ay.dt;
  }
}

void save_results(particule * data, int N, char * filename, int iteration){

}
