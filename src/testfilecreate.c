#include<stdio.h>
#include<stdlib.h>

void CreateFileTest(int nombreParticule, FILE* file){
    int i;
    fprintf(file,"%d\n", nombreParticule);
    for(i = 0; i < nombreParticule; i++){
        fprintf(file,"%f %f %f %f %f\n", i, i, i, i, i);
    }
}

int main(int argc, char** argv){
    if(argc > 2){
        FILE* file = fopen(argv[2], "w+");
        CreateFileTest(atoi(argv[1]), file);
    }
    else fprintf(stdout,"<nombre de particules> <nom du fichier>");
}