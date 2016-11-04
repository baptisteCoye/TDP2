README.md
Auteurs : Baptiste Coye - Mathieu Deschamps

Pour compiler le projet (depuis la source) : 
     Créer un répertoire build.
     Depuis ce répertoire, lancer "cmake ..", puis "make"
     Des flags de compilation sont disponibles dans le CMakeCache.txt (utiliser ccmake pour les modifier) : SAVE_RESULTS (ON pour créer les fichiers résultats, OFF pour ne pas les créer) et PROJET_VERBOSE (0 pour ne pas avoir d'informations, 1 pour quelques input/output et 2 pour le debug)
 

Pour tester le projet :
     Dans le répertoire /build/tests, un exécutable existe :
     	  test_sequ : lance un test en séquentiel en affichant les performances. Ne nécessite pas d'arguments.
     	Ce test utilise le fichier d'entree "/tests/data/file_test.txt" et lance le programme sur 100 itérations.

     Dans le répertoire /build/driver, un exécutable "driver" est créé. Il nécessite en entrée le nombre d'itérations souhaitées ainsi que le fichier contenant la liste des particules. 
     Pour l'utiliser sur un processeur seul : ./driver <N> <filename> 
     Pour l'utiliser sur plusieurs processeurs : mpiexec -n <nbProc> ./driver <N> <filename>