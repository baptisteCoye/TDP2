#include<stdio.h>
#include<stdlib.h>

void CreateFileTest(int nombreParticule, FILE* file){
    int i;
    fprintf(file,"%d\n", nombreParticule);
    for(i = 0; i < nombreParticule; i++){
        fprintf(file,"%f %f %f %f %f %f %f", i, i, i, i, i, i, i);
    }
}

int main(int argc, char** argv){
    if(argc > 2){
        FILE* file = fopen(argv[2], "rw");
        CreateFileTest(atoi(argv[1]), argv[2]);
    }
    else fprintf(stdout,"<nombre de particules> <nom du fichier>");
}